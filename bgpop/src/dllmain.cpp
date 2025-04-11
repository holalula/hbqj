#include <atomic>
#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <format>
#include <windows.h>
#include <mhook-lib/mhook.h>

typedef HRESULT(__fastcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

std::atomic<bool> g_cleanup_requested = false;
std::atomic<bool> g_cleanup_completed = false;
IDXGISwapChainPresent g_present = nullptr;
bool g_initialized = false;
ID3D11Device* g_d3d_device = nullptr;
ID3D11DeviceContext* g_d3d_device_context = nullptr;
ID3D11RenderTargetView* g_main_render_target_view = nullptr;
WNDPROC g_original_wndproc = nullptr;
HWND g_hwnd = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void log(const char* str) {
	std::string msg = std::format("[bgpop] {}", str);
	OutputDebugStringA(msg.c_str());
}

uintptr_t GetPresentAddress() {
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = DefWindowProcW;
	wc.hInstance = GetModuleHandleW(nullptr);
	wc.lpszClassName = L"BGPopD3D";

	DWORD error;
	if (!RegisterClassExW(&wc)) {
		error = GetLastError();
		if (error != ERROR_CLASS_ALREADY_EXISTS) {
			log(std::format("Failed to register window class. Error: {:#x}", error).c_str());
			return 0;
		}
	}

	HWND window = CreateWindowExW(
		0, L"BGPopD3D", L"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0,
		nullptr, nullptr, wc.hInstance, nullptr
	);
	if (!window) return 0;

	ShowWindow(window, SW_HIDE);
	UpdateWindow(window);

	// Handle pending messages
	MSG msg;
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 1;
	sd.BufferDesc.Height = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ID3D11Device* device = nullptr;
	IDXGISwapChain* swap_chain = nullptr;
	D3D_FEATURE_LEVEL feature_level;

	// Specify feature levels explicitly
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	// Create D3D11 device and swap chain
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&feature_level,
		nullptr
	);

	if (FAILED(hr)) {
		log(std::format("D3D11CreateDeviceAndSwapChain failed with HRESULT {:08x}", (uint64_t)hr).c_str());
		DestroyWindow(window);
		return 0;
	}

	// Extract the vtable from the swap chain
	void** vtable = *reinterpret_cast<void***>(swap_chain);
	// Present is the 8th entry
	auto present_addr = reinterpret_cast<uintptr_t>(vtable[8]);

	// Cleanup
	swap_chain->Release();
	device->Release();
	DestroyWindow(window);
	UnregisterClassW(L"BGPopD3D", wc.hInstance);

	return present_addr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui::GetIO().WantCaptureMouse) {
        switch (msg) {
            case WM_LBUTTONDOWN: case WM_LBUTTONUP:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP:
            case WM_MBUTTONDOWN: case WM_MBUTTONUP:
            case WM_MOUSEWHEEL: case WM_MOUSEMOVE:
            case WM_MOUSEHOVER: case WM_MOUSELEAVE:
                ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
                // message has been processed
                return 0;
            default:;
        }
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        switch (msg) {
            case WM_KEYDOWN: case WM_KEYUP:
            case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            case WM_CHAR: case WM_SYSCHAR:
            case WM_IME_CHAR:
                ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
                // message has been processed
                return 0;
            default:;
        }
    }

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }

    return CallWindowProc(g_original_wndproc, hwnd, msg, wparam, lparam);
}

void CreateRenderTarget(IDXGISwapChain* swap_chain) {
    ID3D11Texture2D* back_buffer;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (back_buffer) {
        g_d3d_device->CreateRenderTargetView(back_buffer, nullptr, &g_main_render_target_view);
        back_buffer->Release();
    }
}

void CleanupRenderTarget() {
    if (g_main_render_target_view) {
        g_main_render_target_view->Release();
        g_main_render_target_view = nullptr;
    }
}

void InitializeImGui(IDXGISwapChain* swap_chain) {
    g_d3d_device->GetImmediateContext(&g_d3d_device_context);
    CreateRenderTarget(swap_chain);

    // initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    DXGI_SWAP_CHAIN_DESC sd;
    swap_chain->GetDesc(&sd);
    ImGui_ImplWin32_Init(sd.OutputWindow);

    g_hwnd = sd.OutputWindow;

    // hook WndProc
    g_original_wndproc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

    ImGui_ImplDX11_Init(g_d3d_device, g_d3d_device_context);

    log("Initialize ImGui successfully.");
}

void CleanupImGui() {
    if (g_original_wndproc && g_hwnd) {
        log("Restoring original WndProc...");
        SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_original_wndproc));
        g_original_wndproc = nullptr;
        g_hwnd = nullptr;
        log("Restored original WndProc successfully.");
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupRenderTarget();
    if (g_d3d_device_context) {
        g_d3d_device_context->Release();
        g_d3d_device_context = nullptr;
    }
    if (g_d3d_device) {
        g_d3d_device->Release();
        g_d3d_device = nullptr;
    }

    log("CleanupImGui successfully.");
}

HRESULT __stdcall PresentHook(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    log("Call PresentHook.");

    if (g_cleanup_requested) {
        log("start cleanup...");

        if (g_present) {
            Mhook_Unhook(reinterpret_cast<PVOID*>(&g_present));
            log("Unhook Present().");
        }

        g_cleanup_completed = true;
        log("complete cleanup, call original Present().");
        return g_present(swap_chain, sync_interval, flags);
    }

    if (!g_initialized) {
        log("Initializing...");
        swap_chain->GetDevice(IID_PPV_ARGS(&g_d3d_device));
        if (g_d3d_device) {
            log("Get Device, initializing ImGui...");
            InitializeImGui(swap_chain);

            g_initialized = true;
        }
    }

    bool is_demo_window_open = true;

    if (g_initialized) {
        log("Render ImGui...");
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(&is_demo_window_open);

        ImGui::Render();
        g_d3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    log("Call original Present().");
    return g_present(swap_chain, sync_interval, flags);
}

DWORD WINAPI InitD3D(LPVOID) {
    log("Initialize COM for this thread");
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr)) {
		log("Failed to initialize COM");
        return 1;
	}

    log("GetPresentAddress...");
	auto addr = GetPresentAddress();
	if (addr) {
		g_present = reinterpret_cast<IDXGISwapChainPresent>(addr);
		log(std::format("Present found at: {:#x}", (uint64_t)addr).c_str());
	} else {
		log("Present not found.");
        return 1;
	}

    if (g_present != nullptr) {
        log("Hook Present()...");
        Mhook_SetHook(reinterpret_cast<PVOID*>(&g_present),
                      reinterpret_cast<PVOID>(PresentHook));

    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		log("DLL_PROCESS_ATTACH");
		CreateThread(nullptr, 0, InitD3D, nullptr, 0, nullptr);

		break;
	}
	case DLL_PROCESS_DETACH: {
		log("DLL_PROCESS_DETACH");

        g_cleanup_requested = true;

        for (int i = 0; i < 50 && !g_cleanup_completed; i++) {
            Sleep(100);
            log("waiting for cleanup...");
        }

        if (!g_cleanup_completed) {
            log("WARNING: Cleanup timeout!");
        } else {
            log("Cleanup completed successfully.");
            if (g_initialized) {
                CleanupImGui();
            }
        }

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}

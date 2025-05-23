#pragma once

#include "global_state.h"
#include "logger.h"

namespace hbqj {
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

        ID3D11Device *device = nullptr;
        IDXGISwapChain *swap_chain = nullptr;
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
            log(std::format("D3D11CreateDeviceAndSwapChain failed with HRESULT {:08x}", (uint64_t) hr).c_str());
            DestroyWindow(window);
            return 0;
        }

        // Extract the vtable from the swap chain
        void **vtable = *reinterpret_cast<void ***>(swap_chain);
        // Present is the 8th entry
        auto present_addr = reinterpret_cast<uintptr_t>(vtable[8]);

        // Cleanup
        swap_chain->Release();
        device->Release();
        DestroyWindow(window);
        UnregisterClassW(L"BGPopD3D", wc.hInstance);

        return present_addr;
    }
}
#pragma once

#include <atomic>
#include <d3d11.h>
#include <dxgi.h>

namespace hbqj {
    typedef HRESULT(__fastcall *IDXGISwapChainPresent)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);

namespace state {
    static std::atomic<bool> g_cleanup_requested = false;
    static std::atomic<bool> g_cleanup_completed = false;
    static IDXGISwapChainPresent g_present = nullptr;
    static bool g_initialized = false;
    static ID3D11Device *g_d3d_device = nullptr;
    static ID3D11DeviceContext *g_d3d_device_context = nullptr;
    static ID3D11RenderTargetView *g_main_render_target_view = nullptr;
    static WNDPROC g_original_wndproc = nullptr;
    static HWND g_hwnd = nullptr;
}

}
#include <atomic>
#include <format>
#include <windows.h>
#include <mhook-lib/mhook.h>
#include <memory>

#include "game_camera.h"
#include "game_memory.h"
#include "imgui_manager.h"
#include "preview_housing.h"
#include "load_housing.h"
#include "logger.h"
#include "d3d_manager.h"
#include "process.h"
#include "signature_manager.h"

namespace hbqj {
    DWORD WINAPI InitializeBgpop(LPVOID) {
        log("Initialize COM for this thread");
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
            log("Failed to initialize COM");
            return 1;
        }

        log("GetPresentAddress...");
        auto addr = GetPresentAddress();
        if (addr) {
            state::g_present = reinterpret_cast<IDXGISwapChainPresent>(addr);
            log(std::format("Present found at: {:#x}", (uint64_t) addr).c_str());
        } else {
            log("Present not found.");
            return 1;
        }

        if (state::g_present != nullptr) {
            log("Hook Present()...");
            Mhook_SetHook(reinterpret_cast<PVOID *>(&state::g_present),
                          reinterpret_cast<PVOID>(PresentHook));

        }

        auto process = std::make_shared<Process>();
        // log("Init SM.");
        // SignatureManager sm_;
        // sm_.Initialize(process);
        // log("Done.");

        g_get_view_matrix_func = reinterpret_cast<GetViewMatrixFunc>
        (process->GetBaseAddr() + get_view_matrix_func_offset);
        Mhook_SetHook(reinterpret_cast<PVOID *>(&g_get_view_matrix_func),
                      reinterpret_cast<PVOID>(GetViewMatrixHook));


        g_get_active_camera_func = reinterpret_cast<GetActiveCameraFunc>
        (process->GetBaseAddr() + g_get_active_camera_offset);
        Mhook_SetHook(reinterpret_cast<PVOID *>(&g_get_active_camera_func),
                      reinterpret_cast<PVOID>(GetActiveCameraHook));

        PreviewHousing::load_housing_func_offset = 0xC4A390;
        PreviewHousing::load_housing_func = reinterpret_cast<LoadHousingFunc>
                (process->GetBaseAddr() + PreviewHousing::load_housing_func_offset);
        Mhook_SetHook(reinterpret_cast<PVOID *>(&PreviewHousing::load_housing_func),
                      reinterpret_cast<PVOID>(PreviewHousing::LoadHousingFuncHook));

        LoadHousing::select_item_func_offset = 0x6D1520;
        LoadHousing::select_item_func = reinterpret_cast<SelectItemFunc>
                (process->GetBaseAddr() + LoadHousing::select_item_func_offset);
        Mhook_SetHook(reinterpret_cast<PVOID *>(&LoadHousing::select_item_func),
                      reinterpret_cast<PVOID>(LoadHousing::SelectItemFuncHook));

        LoadHousing::place_item_func_offset = 0x6D1E80;
        LoadHousing::place_item_func = reinterpret_cast<PlaceItemFunc>
                (process->GetBaseAddr() + LoadHousing::place_item_func_offset);
        Mhook_SetHook(reinterpret_cast<PVOID *>(&LoadHousing::place_item_func),
                      reinterpret_cast<PVOID>(LoadHousing::PlaceItemFuncHook));

        memory.Initialize(process);

        return 0;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            hbqj::log("DLL_PROCESS_ATTACH");
            CreateThread(nullptr, 0, hbqj::InitializeBgpop, nullptr, 0, nullptr);

            break;
        }
        case DLL_PROCESS_DETACH: {
            hbqj::log("DLL_PROCESS_DETACH");

            hbqj::state::g_cleanup_requested = true;

            for (int i = 0; i < 50 && !hbqj::state::g_cleanup_completed; i++) {
                Sleep(100);
                hbqj::log("waiting for cleanup...");
            }

            if (!hbqj::state::g_cleanup_completed) {
                hbqj::log("WARNING: Cleanup timeout!");
            } else {
                hbqj::log("Cleanup completed successfully.");
                if (hbqj::state::g_initialized) {
                    hbqj::CleanupImGui();
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

﻿#include <atomic>
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
#include "game_process.h"
#include "signature_manager.h"
#include "ipc/poller.h"

namespace hbqj {
    std::unique_ptr<Poller> poller;

    std::unique_ptr<SignatureManager> signature_manager;

    void OnHbqjEvent(SharedMemory *sm, EventType type) {
        sm->data2 = sm->data1 + 300;

        log(std::format("Receive Event, type: {}", static_cast<int>(type)).c_str());

        switch (type) {
            case EventType::UpdateImGuizmoFlag: {
                g_imguizmo_on = sm->imguizmo_on;
                break;
            }
            case EventType::PreviewHousingLayout: {
                std::vector<HousingItem> items;
                for (int i = 0; i < sm->preview_items_count; i++) {
                    items.push_back({
                                            .type = sm->preview_items[i].type,
                                            .position = sm->preview_items[i].position,
                                            .rotation = sm->preview_items[i].rotation,
                                            .color = sm->preview_items[i].color,
                                    });
                }

                PreviewHousing::PreviewHousingLayout(
                        items,
                        sm->preview_items_count
                );
                break;
            }
            case EventType::LoadHousingLayout: {
                if (memory.initialized) {
                    const auto &addr = memory.GetHousingStructureAddr();

                    const auto &items = sm->load_layout_items;

                    if (addr) {
                        std::thread load_layout_thread([addr, items] {
                            for (const auto &item: items) {
                                LoadHousing::select_item_func(addr.value(),
                                                              static_cast<int64_t>(item.item_addr));

                                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                                memory.SetActivePosition(item.position.x, item.position.y,
                                                         item.position.z);

                                memory.SetActiveRotation(item.rotation);

                                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                                LoadHousing::place_item_func(addr.value(),
                                                             static_cast<int64_t>(item.item_addr));

                                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                            }
                        });

                        load_layout_thread.detach();
                    }
                }
                break;
            }
        }
    }

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

        signature_manager = std::make_unique<SignatureManager>();
        signature_manager->Initialize(process);

        const auto get_view_matrix_func_offset_result = signature_manager->GetSignature(SignatureType::ViewMatrix)
                .transform([](auto signature) { return signature->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); })
                .and_then([&process](auto addr) { return process->CalculateTargetOffsetCall(addr); });

        if (get_view_matrix_func_offset_result) {
            log(std::format("Hook GetViewMatrixFunc at offset: {:x}",
                            get_view_matrix_func_offset_result.value()).c_str());

            g_get_view_matrix_func = reinterpret_cast<GetViewMatrixFunc>
            (process->GetBaseAddr() + get_view_matrix_func_offset_result.value());

            Mhook_SetHook(reinterpret_cast<PVOID *>(&g_get_view_matrix_func),
                          reinterpret_cast<PVOID>(GetViewMatrixHook));
        }

        const auto get_active_camera_offset = signature_manager->GetSignature(SignatureType::GetActiveCamera)
                .transform([](auto sig) { return sig->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); })
                .and_then([&process](auto addr) { return process->CalculateTargetOffsetCall(addr); });

        if (get_active_camera_offset) {
            g_get_active_camera_func = reinterpret_cast<GetActiveCameraFunc>
            (process->GetBaseAddr() + get_active_camera_offset.value());
            Mhook_SetHook(reinterpret_cast<PVOID *>(&g_get_active_camera_func),
                          reinterpret_cast<PVOID>(GetActiveCameraHook));
        }

        const auto load_house_offset = signature_manager->GetSignature(SignatureType::LoadHouse)
                .transform([](auto sig) { return sig->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); });

        if (load_house_offset) {
            PreviewHousing::load_housing_func = reinterpret_cast<LoadHousingFunc>
            (process->GetBaseAddr() + load_house_offset.value());
            Mhook_SetHook(reinterpret_cast<PVOID *>(&PreviewHousing::load_housing_func),
                          reinterpret_cast<PVOID>(PreviewHousing::LoadHousingFuncHook));
        }

        const auto select_item_func_offset = signature_manager->GetSignature(SignatureType::SelectHousingItem)
                .transform([](auto sig) { return sig->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); });

        if (select_item_func_offset) {
            LoadHousing::select_item_func = reinterpret_cast<SelectItemFunc>
            (process->GetBaseAddr() + select_item_func_offset.value());
            Mhook_SetHook(reinterpret_cast<PVOID *>(&LoadHousing::select_item_func),
                          reinterpret_cast<PVOID>(LoadHousing::SelectItemFuncHook));
        }

        const auto place_item_func_offset = signature_manager->GetSignature(SignatureType::PlaceHousingItem)
                .transform([](auto sig) { return sig->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); });

        if (place_item_func_offset) {
            LoadHousing::place_item_func = reinterpret_cast<PlaceItemFunc>
            (process->GetBaseAddr() + place_item_func_offset.value());
            Mhook_SetHook(reinterpret_cast<PVOID *>(&LoadHousing::place_item_func),
                          reinterpret_cast<PVOID>(LoadHousing::PlaceItemFuncHook));
        }

        memory.Initialize(process);

        poller = std::move(std::make_unique<Poller>(OnHbqjEvent));

        if (poller->Start()) {
            log("Start Poller Thread..");
        } else {
            log("Failed to Start Poller Thread..");
        }

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
                Sleep(10);
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

            if (hbqj::poller) {
                hbqj::log("Start stopping Poller..");
                hbqj::poller->Stop();
                hbqj::poller.reset();
                hbqj::log("Poller Thread Exists..");
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

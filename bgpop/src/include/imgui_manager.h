#pragma once

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <ImGuizmo.h>

#include "d3d_manager.h"
#include "game_memory.h"
#include "global_state.h"
#include "math_utils.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hbqj {
    static bool g_resized = false;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        if (ImGui::GetIO().WantCaptureMouse) {
            switch (msg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MOUSEWHEEL:
                case WM_MOUSEMOVE:
                case WM_MOUSEHOVER:
                case WM_MOUSELEAVE:
                    ::ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
                    // message has been processed
                    return 0;
                default:;
            }
        }

        if (ImGui::GetIO().WantCaptureKeyboard) {
            switch (msg) {
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_CHAR:
                case WM_SYSCHAR:
                case WM_IME_CHAR:
                    ::ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
                    // message has been processed
                    return 0;
                default:;
            }
        }

        if (::ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
            return true;
        }

        switch (msg) {
            case WM_SIZE:
                if (wparam != SIZE_MINIMIZED) {
                    g_resized = true;
                    // The game process may call ResizeBuffers to handle window resize, we need to clean the created
                    // RenderTargetView (which is bound to device context by OMSetRenderTargets) before the
                    // ResizeBuffers is called, otherwise the call would fail.
                    // https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
                    // Note: not mutate render resources in message handler thread.
                }
        }

        return CallWindowProc(state::g_original_wndproc, hwnd, msg, wparam, lparam);
    }

    void CreateRenderTarget(IDXGISwapChain *swap_chain) {
        ID3D11Texture2D *back_buffer;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        if (back_buffer) {
            D3D11_TEXTURE2D_DESC desc;
            back_buffer->GetDesc(&desc);

            log(std::format("Back Buffer Format: {}, Samples: {}", (int) desc.Format, desc.SampleDesc.Count).c_str());

            state::g_d3d_device->CreateRenderTargetView(back_buffer, nullptr, &state::g_main_render_target_view);
            back_buffer->Release();
        }
    }

    void CleanupRenderTarget() {
        if (state::g_main_render_target_view) {
            state::g_main_render_target_view->Release();
            state::g_main_render_target_view = nullptr;
        }
    }

    void InitializeMatrices();

    void InitializeImGui(IDXGISwapChain *swap_chain) {
        state::g_d3d_device->GetImmediateContext(&state::g_d3d_device_context);
        CreateRenderTarget(swap_chain);

        // initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        DXGI_SWAP_CHAIN_DESC sd;
        swap_chain->GetDesc(&sd);
        ImGui_ImplWin32_Init(sd.OutputWindow);

        state::g_hwnd = sd.OutputWindow;

        // hook WndProc
        state::g_original_wndproc = reinterpret_cast<WNDPROC>(
                SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

        ImGui_ImplDX11_Init(state::g_d3d_device, state::g_d3d_device_context);

        InitializeMatrices();
        log("Initialize ImGui successfully.");
    }

    void CleanupImGui() {
        if (state::g_original_wndproc && state::g_hwnd) {
            log("Restoring original WndProc...");
            SetWindowLongPtr(state::g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(state::g_original_wndproc));
            state::g_original_wndproc = nullptr;
            state::g_hwnd = nullptr;
            log("Restored original WndProc successfully.");
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupRenderTarget();
        if (state::g_d3d_device_context) {
            state::g_d3d_device_context->Release();
            state::g_d3d_device_context = nullptr;
        }
        if (state::g_d3d_device) {
            state::g_d3d_device->Release();
            state::g_d3d_device = nullptr;
        }

        log("CleanupImGui successfully.");
    }

    float g_objectMatrix[16] = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
    };

    float g_view[16] = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
    };

    float g_proj[16] = {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
    };

    float objectMatrix[4][16] = {
            {1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f},

            {1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    2.f, 0.f, 0.f, 1.f},

            {1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    2.f, 0.f, 2.f, 1.f},

            {1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 2.f, 1.f}
    };

    float matrixTranslation[3] = {1.0f, 1.0f, 1.0f},
            matrixRotation[3] = {1.0f, 1.0f, 1.0f},
            matrix_scale[3] = {1.0f, 1.0f, 1.0f};
    float view_projection_matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f},
            identity_matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f},
            item_matrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f};

    void InitializeMatrices() {
        float eye[] = {5.0f, 5.0f, 5.0f};  // camera position (5,5,5)
        float at[] = {0.0f, 0.0f, 0.0f};   // look at origin
        float up[] = {0.0f, 1.0f, 0.0f};   // up direction
        LookAt(eye, at, up, g_view);

        ImGuiIO &io = ImGui::GetIO();
        Perspective(45.0f, io.DisplaySize.x / io.DisplaySize.y, 1.0f, 100.0f, g_proj);
    }

    float item_position[3] = {.0f, .0f, .0f};
    float item_rotation[3] = {.0f, .0f, .0f};
    Matrix4x4 projection_m;
    Matrix4x4 view_m;

    void DrawImGuizmo() {
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Gizmo", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoInputs);

        ImGuizmo::Enable(true);

        ImGuizmo::BeginFrame();

        ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                          ImGui::GetWindowPos().y,
                          ImGui::GetWindowWidth(),
                          ImGui::GetWindowHeight());

        ImGuizmo::SetOrthographic(false);

        ImGuizmo::SetDrawlist();

//        ImGuizmo::Manipulate(
//                g_view,
//                g_proj,
//                ImGuizmo::TRANSLATE,
//                ImGuizmo::LOCAL,
//                g_objectMatrix
//        );

//        if (active_camera) {
//            log(std::format("Active Camera addr: {:x}", reinterpret_cast<int64_t>(active_camera)).c_str());
//            if (active_camera && active_camera->scene_camera.render_camera) {
//                log(std::format("Render Camera addr: {:x}", reinterpret_cast<int64_t>(active_camera->scene_camera.render_camera)).c_str());
//                log(std::format("Far: {}", active_camera->scene_camera.render_camera->far_plane).c_str());
//                log(std::format("Near: {}", active_camera->scene_camera.render_camera->near_plane).c_str());
//                log(std::format("Project Matrix addr: {:x}", reinterpret_cast<int64_t>(&active_camera->scene_camera.render_camera->projection_matrix)).c_str());
//            }
//        }

        // ImGuizmo::DrawCubes(g_view, g_proj, &objectMatrix[0][0], 1);

        // refer to:
        // https://github.com/LeonBlade/BDTHPlugin/blob/main/BDTHPlugin/Interface/Gizmo.cs
        for (int i = 0; i < 16; i++) {
            projection_m.matrix[i] = active_camera->scene_camera.render_camera->projection_matrix.matrix[i];
        }
        for (int i = 0; i < 16; i++) {
            view_m.matrix[i] = active_camera->scene_camera.viewMatrix.matrix[i];
        }

        float far_plane = active_camera->scene_camera.render_camera->far_plane;
        float near_plane = active_camera->scene_camera.render_camera->near_plane;
        float clip = far_plane / (far_plane - near_plane);
        projection_m.m43 = -(clip * near_plane);
        projection_m.m33 = -((far_plane + near_plane) / (far_plane - near_plane));

        view_m.m44 = 1.f;

        if (ImGuizmo::Manipulate(
                // view_projection_matrix,
                view_m.matrix,
                // identity_matrix,
                projection_m.matrix,
                ImGuizmo::TRANSLATE,
                ImGuizmo::LOCAL,
                item_matrix,
                nullptr,
                nullptr,
                nullptr,
                nullptr)) {
            ImGuizmo::DecomposeMatrixToComponents(
                    item_matrix,
                    item_position,
                    item_rotation,
                    matrix_scale
            );

            log(std::format("Update position to: {:.2f}, {:.2f}, {:.2f}",
                            item_position[0],
                            item_position[1],
                            item_position[2]
            ).c_str());
            memory.SetActivePosition(
                    item_position[0],
                    item_position[1],
                    item_position[2]
            );
        }

        ImGui::End();
    }

    HRESULT __stdcall PresentHook(IDXGISwapChain *swap_chain, UINT sync_interval, UINT flags) {
        // log("Call PresentHook.");

        if (state::g_cleanup_requested) {
            log("start cleanup...");

            if (state::g_present) {
                Mhook_Unhook(reinterpret_cast<PVOID *>(&state::g_present));
                log("Unhook Present().");
            }

            // TODO: clean up this hook elsewhere
            if (g_get_view_matrix_func) {
                Mhook_Unhook(reinterpret_cast<PVOID *>(&g_get_view_matrix_func));
                log("Unhook GetViewMatrix().");
            }
            if (g_get_active_camera_func) {
                Mhook_Unhook(reinterpret_cast<PVOID *>(&g_get_active_camera_func));
                log("Unhook GetActiveCamera().");
            }

            state::g_cleanup_completed = true;
            log("complete cleanup, call original Present().");
            return state::g_present(swap_chain, sync_interval, flags);
        }

        if (!state::g_initialized) {
            log("Initializing...");
            swap_chain->GetDevice(IID_PPV_ARGS(&state::g_d3d_device));
            if (state::g_d3d_device) {
                log("Get Device, initializing ImGui...");
                InitializeImGui(swap_chain);

                state::g_initialized = true;
            }
        }

        bool is_demo_window_open = true;

        if (state::g_initialized) {
            // log("Render ImGui...");

            // log(std::format("g_view_matrix_addr: 0x{:x}", g_view_matrix_addr).c_str());
            // if (g_view_matrix) {
            //     log(std::format("{}, {}, {}, {}",
            //                     g_view_matrix->matrix[0],
            //                     g_view_matrix->matrix[1],
            //                     g_view_matrix->matrix[14],
            //                     g_view_matrix->matrix[15]
            //     ).c_str());
            // }

            if (g_resized) {
                log("Handle window resize, cleanup ImGui resources and re-initialize in next Present call");
                // The swap_chain in this call is not resized yet, need to initialize ImGui with the resized
                // swap_chain in next call.
                // TODO: can we just clean RTV instead of all resources?
                CleanupImGui();
                state::g_initialized = false;
                g_resized = false;
                return state::g_present(swap_chain, sync_interval, flags);
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // ImGui::ShowDemoWindow(&is_demo_window_open);
            if (memory.initialized && g_view_matrix) {
                auto layout_mode = memory.GetLayoutMode().value_or(-1);
                if (layout_mode == HousingLayoutMode::Rotate) {
                    const auto &pos = memory.GetActivePosition();
                    const auto &rot = memory.GetActiveRotation();
                    if (pos.has_value() && rot.has_value()) {
                        log(std::format("{}, {}", pos.value(), rot.value()).c_str());

                        item_position[0] = pos.value().x;
                        item_position[1] = pos.value().y;
                        item_position[2] = pos.value().z;

                        const auto &rotations = QuaternionToEulerAngles(rot.value());
                        for (int i = 0; i < 3; i++) {
                            item_rotation[i] = rotations[i];
                        }

                        ImGuizmo::RecomposeMatrixFromComponents(
                                item_position,
                                item_rotation,
                                matrix_scale,
                                item_matrix);

                        for (int i = 0; i < 16; i++) {
                            view_projection_matrix[i] = g_view_matrix->matrix[i];
                        }

                        DrawImGuizmo();
                    }
                }
            }


            // DrawImGuizmo();

            ImGui::Render();
            state::g_d3d_device_context->OMSetRenderTargets(1, &state::g_main_render_target_view, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

        // log("Call original Present().");
        return state::g_present(swap_chain, sync_interval, flags);
    }
}
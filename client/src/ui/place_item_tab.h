#pragma once

#include "ui.h"
#include "ipc/heart_beat.h"
#include "ipc/process_resources.h"

namespace hbqj {
    static bool place_anywhere = false;
    static bool imguizmo = false;
    static float pos_x = 0.0f;
    static float pos_y = 0.0f;
    static float pos_z = 0.0f;
    static float pos_step = 0.1f;

    float left_indent = 13.0f;

    void SetActiveItemPosition(const std::shared_ptr<Memory> &memory,
                               const std::optional<float> &x,
                               const std::optional<float> &y,
                               const std::optional<float> &z) {
        if (memory->initialized) {
            auto mode_result = memory->GetLayoutMode();
            if (mode_result && mode_result.value() == HousingLayoutMode::Rotate) {
                memory->SetActivePosition(x, y, z);
            }
        }
    }

    void PlaceHousingItemTab() {
        ImGui::Indent(left_indent);

        const auto &memory = HeartBeatMonitor::GetInstance().GetMemoryOperation();

        if (ImGui::Checkbox("PlaceAnywhere", &place_anywhere)) {
            log("Place anywhere: {}", place_anywhere);
            if (memory->initialized) {
                memory->PlaceAnywhere(place_anywhere);
            }
        }

        if (ImGui::Checkbox("ImGuizmo", &imguizmo)) {
            log("ImGuizmo: {}", imguizmo);
            if (memory->initialized) {
                const auto &r = ProcessResources::GetInstance();
                r.GetSharedMemory()->imguizmo_on = imguizmo;
                SetEvent(r.events_.update_imguizmo_flag.get());
            }
        }

        if (memory->initialized) {
            auto mode_result = memory->GetLayoutMode();
            if (mode_result && mode_result.value() == HousingLayoutMode::Rotate) {
                const auto &position_result = memory->GetActivePosition();
                if (position_result) {
                    const auto &position = position_result.value();
                    pos_x = position.x;
                    pos_y = position.y;
                    pos_z = position.z;
                }
            }
        }

        if (ImGui::InputFloat("X", &pos_x, pos_step, 0, "%.3f")) {
            SetActiveItemPosition(memory, pos_x, std::nullopt, std::nullopt);
        }

        if (ImGui::InputFloat("Y", &pos_y, pos_step, 0, "%.3f")) {
            SetActiveItemPosition(memory, std::nullopt, pos_y, std::nullopt);
        }

        if (ImGui::InputFloat("Z", &pos_z, pos_step, 0, "%.3f")) {
            SetActiveItemPosition(memory, std::nullopt, std::nullopt, pos_z);
        }

        ImGui::InputFloat("Step", &pos_step, 0.01, 0.01, "%.2f");

        ImGui::Unindent(left_indent);
    }
}

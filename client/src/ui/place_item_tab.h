#pragma once

#include "ui.h"
#include "ipc/heart_beat.h"

namespace hbqj {
    static bool place_anywhere = false;
    static bool imguizmo = false;
    static float pos_x = 0.0f;
    static float pos_y = 0.0f;
    static float pos_z = 0.0f;
    static float pos_step = 0.1f;

    float left_indent = 13.0f;

    void PlaceHousingItemTab() {
        ImGui::Indent(left_indent);

        if (ImGui::Checkbox("PlaceAnywhere", &place_anywhere)) {
            log("Place anywhere: {}", place_anywhere);
            const auto &memory = HeartBeatMonitor::GetInstance().GetMemoryOperation();
            if (memory->initialized) {
                memory->PlaceAnywhere(place_anywhere);
            }
        }

        if (ImGui::Checkbox("ImGuizmo", &imguizmo)) {
            log("ImGuizmo: {}", imguizmo);
        }

        ImGui::InputFloat("X", &pos_x, pos_step, 0, "%.3f");
        ImGui::InputFloat("Y", &pos_y, pos_step, 0, "%.3f");
        ImGui::InputFloat("Z", &pos_z, pos_step, 0, "%.3f");

        ImGui::InputFloat("Step", &pos_step, 0.01, 0.01, "%.2f");

        ImGui::Unindent(left_indent);
    }
}

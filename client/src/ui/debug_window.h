#pragma once

#include "ui.h"

void DebugWindow() {
    ImGui::SetNextWindowSize({400, 400});
    ImGui::Begin("Debug");

    ImGui::Text("中文测试");

    if (ImGui::SliderFloat("Scale Bar", &g_scale, SCALE_MIN, SCALE_MAX)) {
        log("Scale in Slider bar: {}", g_scale);
        g_pending_scale = true;
    }

    if (ImGui::InputFloat("Scale", &g_scale)) {
        g_pending_scale = true;
    }

    ImGui::End();
}
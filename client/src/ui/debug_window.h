#pragma once

#include <iostream>

#include "ui.h"

char parameter_1[32] = "0x1234";

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

    if (ImGui::InputTextMultiline("Parameter1", parameter_1, sizeof(parameter_1), {100, 100},
                                  ImGuiInputTextFlags_EnterReturnsTrue)) {
        try {
            std::string hex_str(parameter_1);
            if (hex_str.substr(0, 2) == "0x") {
                hex_str = hex_str.substr(2);
            }
            std::cout << std::stoull(hex_str, nullptr, 16) << std::endl;
        } catch (const std::exception &e) {
            std::cout << "Invalid hex number" << std::endl;
        }
    }

    ImGui::End();
}
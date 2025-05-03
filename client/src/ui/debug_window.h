#pragma once

#include <iostream>
#include <nlohmann/json.hpp>

#include "ui.h"

namespace hbqj {
    char parameter_1[32] = "0x1234";

    using json = nlohmann::json;

    void CatchExceptions() {
        // If /EHs is not added in MSVC (which means the stack unwinding is not enabled),
        // The first runtime exception can be caught, while the json exception can't.
        // The second invalid json exception causes the process to exit and the error message is
        // > Process finished with exit code -1073740791 (0xC0000409)
        // 0xC0000409 represents STATUS_STACK_BUFFER_OVERRUN, which is usually triggered by
        // a stack buffer overflow (such as array out of range), and this could be caused by no stack unwinding.

        if (ImGui::Button("Runtime Exception")) {
            try {
                throw std::runtime_error("ex");
            } catch (...) {
                log("Catch ex");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Json Exception")) {
            try {
                std::string invalid_json = "{\"key\": invalid}";
                auto j = json::parse(invalid_json);
            } catch (...) {
                log("Catch json ex");
            }
        }
    }

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

        CatchExceptions();

        ImGui::End();
    }
}
#pragma once

#include <format>
#include <filesystem>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <tchar.h>

// State
static int WINDOW_WIDTH = 600;
static int WINDOW_HEIGHT = 400;
static int WINDOW_X = 100;
static int WINDOW_Y = 100;
static bool g_pending_scale = false;
static float g_scale = 1.f;
static constexpr float SCALE_MIN = 0.5f;
static constexpr float SCALE_MAX = 3.0f;
static constexpr float FONT_PIXEL_SIZE = 20.0f;

static const ImWchar font_ranges[] = {
        0x0020, 0x00FF,  // Basic Latin + Latin Supplement (English characters, numbers, common symbols)
        0x3000, 0x30FF,  // CJK Symbols and Punctuation (Chinese punctuation marks)
        0x4e00, 0x9FAF,  // CJK Unified Ideographs (Common Chinese characters)
        0xFF00, 0xFFEF,  // Full-width forms (Full-width numbers and letters commonly used in Chinese text)
        0,
};

static const std::filesystem::path GetSystemFontsDir() {
    char* pValue;
    size_t len;
    errno_t err = _dupenv_s(&pValue, &len, "SYSTEMROOT");
    if (err == 0 && pValue != nullptr) {
        std::filesystem::path path(pValue);
        free(pValue);
        return path / "Fonts";
    }
    // fallback to default Windows fonts directory if environment variable is not available
    return "C:\\Windows\\Fonts";
}

static const std::filesystem::path SYSTEM_FONTS_DIR = GetSystemFontsDir();
static const std::filesystem::path MICROSOFT_YAHEI = SYSTEM_FONTS_DIR / "msyh.ttc";

template<typename ...Args>
void log(std::string_view fmt, Args &&... args) {
    std::cout << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
}

void LoadFont(ImGuiIO &io, float font_pixel_size) {
    log("Load Font with size: {}", font_pixel_size);

    io.Fonts->Clear();

    // TODO: reload Font from memory instead of file
    ImFont* font = io.Fonts->AddFontFromFileTTF(MICROSOFT_YAHEI.string().c_str(),
                                                font_pixel_size,
                                                nullptr,
                                                font_ranges);

    if (font == nullptr) {
        log("Failed to load system font.");
        // back to default font if fail
        font = io.Fonts->AddFontDefault();
    }

    io.Fonts->Build();
}

void ScaleUI(float new_scale) {
    new_scale = ImClamp(new_scale, SCALE_MIN, SCALE_MAX);

    ImGuiIO &io = ImGui::GetIO();

    LoadFont(io, FONT_PIXEL_SIZE * new_scale);

    ImGui_ImplDX11_InvalidateDeviceObjects();

    // default style first
    ImGui::GetStyle() = ImGuiStyle{};

    ImGui::GetStyle().ScaleAllSizes(new_scale);
}

#pragma once

#include <format>

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

template<typename ...Args>
void log(std::string_view fmt, Args &&... args) {
    std::cout << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
}

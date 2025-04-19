#pragma once

#include <format>
#include <string>
#include <windows.h>

namespace hbqj {
    void log(const char *str) {
        std::string msg = std::format("[bgpop] {}", str);
        OutputDebugStringA(msg.c_str());
    }
}
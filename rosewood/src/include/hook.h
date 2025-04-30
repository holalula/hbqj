#pragma once

#include <expected>
#include <string_view>
#include <filesystem>

#include "error.h"
#include "log.h"
#include "game_process.h"
#include "macro.h"

namespace hbqj {
    class __declspec(dllexport) Hook {
    public:
        std::expected<bool, Error> Inject(std::wstring_view dll_path, std::string_view process_name);

        std::expected<bool, Error> SafeInject(const std::filesystem::path &dll_path, std::string_view process_name);

        std::expected<bool, Error> Unload(std::wstring_view dll_name, std::string_view process_name);

        std::expected<std::vector<std::wstring>, Error> GetLoadedModules(std::string_view process_name);

        Process process_;
        Logger log = Logger::GetLogger("Hook");
    private:
    };
}
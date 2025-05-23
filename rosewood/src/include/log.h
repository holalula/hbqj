#pragma once

#include <chrono>
#include <esent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ShlObj_core.h>
#include <unordered_map>

#include "utils/string_utils.h"

namespace hbqj {
    enum class LogLevel {
        INFO,
        WARN,
        ERR,
    };

    enum class LogTarget : uint8_t {
        NONE = 0,
        TERMINAL = 1 << 0,
        FILE = 1 << 1,
        ALL = TERMINAL | FILE
    };

    constexpr LogTarget operator|(LogTarget a, LogTarget b) {
        return static_cast<LogTarget>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    constexpr LogTarget operator&(LogTarget a, LogTarget b) {
        return static_cast<LogTarget>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    class __declspec(dllexport) Logger {
    public:
        static Logger &GetLogger(const std::string &name) {
            static std::unordered_map<std::string, Logger> loggers;
            auto it = loggers.find(name);
            if (it == loggers.end()) {
                it = loggers.emplace(name, Logger(name)).first;
            }
            return it->second;
        }

        static Logger &Default() {
            static Logger &default_logger = GetLogger("Default");
            default_logger.SetTarget(LogTarget::TERMINAL);
            return default_logger;
        }

        template<typename... Args>
        static void Info(std::format_string<Args...> fmt, Args &&... args) {
            Default().info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Error(std::format_string<Args...> fmt, Args &&... args) {
            Default().error(fmt, std::forward<Args>(args)...);
        }

        Logger &SetTarget(LogTarget target) {
            target_ = target;
            return *this;
        }

        Logger &SetLogFile(const std::filesystem::path &path) {
            log_file_path_ = path;
            return *this;
        }

        template<typename... Args>
        void Log(LogLevel level, std::format_string<Args...> fmt, Args &&... args) {
            auto message = std::format(fmt, std::forward<Args>(args)...);
            OutputLog(level, message);
        }

        template<typename... Args>
        void info(std::format_string<Args...> fmt, Args &&... args) {
            Log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warn(std::format_string<Args...> fmt, Args &&... args) {
            Log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void error(std::format_string<Args...> fmt, Args &&... args) {
            Log(LogLevel::ERR, fmt, std::forward<Args>(args)...);
        }

    private:
        Logger(const std::string &name) {
            logger_name_ = std::format("[{}]", name);

            static std::string process_name = []() {
                wchar_t path[MAX_PATH];
                // null represents the path of the executable file of the current process
                GetModuleFileNameW(nullptr, path, MAX_PATH);
                return std::filesystem::path(utf16_to_utf8(path)).filename().stem().string();
            }();

            wchar_t path[MAX_PATH];
            SHGetSpecialFolderPathW(nullptr, path, CSIDL_APPDATA, FALSE);
            std::wstring w_path(path);

            log_file_path_ = std::filesystem::path(w_path) / "hbqj" / "log" /
                             std::format("{}.hbqj.log", process_name);
            std::filesystem::create_directories(log_file_path_.parent_path());
        }

        void OutputLog(LogLevel level, const std::string &message) {
            auto timestamp = GetCurrentTime();
            auto level_string = GetLevelString(level);
            auto full_message = std::format("{} {} {} {}\n", timestamp, level_string, logger_name_, message);

            if (static_cast<uint8_t>(target_ & LogTarget::TERMINAL)) {
                std::cout << full_message;
            }
            if (static_cast<uint8_t>(target_ & LogTarget::FILE)) {
                std::ofstream log_file(log_file_path_, std::ios::app);
                if (log_file) {
                    log_file << full_message;
                }
            }
        }

        std::string GetCurrentTime() {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);

            std::tm tm_buf;
            localtime_s(&tm_buf, &time);

            std::ostringstream oss;
            oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }

        std::string GetLevelString(LogLevel level) {
            switch (level) {
                case LogLevel::INFO:
                    return "[INFO]";
                case LogLevel::WARN:
                    return "[WARN]";
                case LogLevel::ERR:
                    return "[ERROR]";
            }
        }

        LogTarget target_ = LogTarget::ALL;
        std::filesystem::path log_file_path_;
        std::string logger_name_;
    };
}
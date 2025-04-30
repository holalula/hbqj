#pragma once

#include <filesystem>
#include <string>
#include <thread>
#include <windows.h>
#include <TlHelp32.h>

#include "log.h"
#include "game_process.h"
#include "hook.h"

namespace hbqj {
    class HeartBeatMonitor {
    public:
        static HeartBeatMonitor &GetInstance() {
            static HeartBeatMonitor monitor;
            return monitor;
        }

        HeartBeatMonitor(const HeartBeatMonitor &) = delete;

        HeartBeatMonitor &operator=(const HeartBeatMonitor &) = delete;

        void Start() {
            if (running_) return;

            running_ = true;

            heart_beat_thread_ = std::thread(&HeartBeatMonitor::HeartBeatThread, this);

            log.info("Start heart beat thread.");
        }

        void Stop() {
            if (!running_) return;

            running_ = false;

            if (heart_beat_thread_.joinable()) {
                heart_beat_thread_.join();
            }

            // Unload injected dll if needed
            log.info("Stop heart beat thread.");
        }

        bool IsRunning() {
            return running_;
        }

        bool IsGameProcessRunning() {
            return is_game_process_running_;
        }

        bool IsInjected() {
            return is_dll_injected_;
        }

    private:
        std::filesystem::path dll_path_;

        std::string process_name_;

        std::string module_name_;

        unsigned long check_interval_ms_;

        std::atomic<bool> running_;

        std::atomic<bool> is_game_process_running_ = false;

        std::atomic<bool> is_dll_injected_ = false;

        std::thread heart_beat_thread_;

        Process game_process_;

        Hook injector_;

        inline static Logger log = Logger::GetLogger("HeartBeat");

        HeartBeatMonitor() :
                process_name_("ffxiv_dx11.exe"),
                module_name_("namazu.dll"),
                check_interval_ms_(100),
                game_process_(false),
                injector_() {

            // TODO: fix dll path
            dll_path_ = std::filesystem::current_path() / module_name_;

            log.info("DllPath: {}", dll_path_.string());
        }

        void HeartBeatThread() {
            log.info("Launch heart beat thread..");
            while (running_) {
                if (game_process_.GetProcess(process_name_).has_value()) {
                    log.info("Game is launched..");
                    is_game_process_running_ = true;
                    if (!game_process_.GetProcessModule(process_name_, module_name_).has_value()) {
                        is_dll_injected_ = false;
                        log.info("Inject dll to game...");
                        // One interesting issue here is that even if the below code line shouldn't be executed (since
                        // game is not launched), the client exits with error:
                        // > Process finished with exit code -1073741515 (0xC0000135)
                        // 0xC0000135 is STATUS_DLL_NOT_FOUND
                        // But if we replace it with a return statement, this thread won't exit.
                        // It seems this is caused by compiler optimizations, but even all MSVC optimizations are disabled
                        // (use /Od flag), this issue still doesn't go away.
                        // return;
                        auto result = injector_.Inject(dll_path_.wstring(), process_name_);
                        if (result.has_value()) {
                            log.info("Injected {} to {}.", dll_path_.string(), process_name_);
                        } else {
                            log.error("Failed to inject to game process..");
                        }
                    } else {
                        is_dll_injected_ = true;
                    }
                } else {
                    is_game_process_running_ = false;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms_));
            }
        }
    };
}
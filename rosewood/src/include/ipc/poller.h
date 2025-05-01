#pragma once

#include <windows.h>
#include <process.h>
#include <atomic>
#include <thread>
#include <functional>
#include <memory>
#include <chrono>

#include "shared_memory_reader.h"

namespace hbqj {
    class Poller {
    private:
        std::unique_ptr<SharedMemoryReader> reader_;
        std::thread polling_thread_;
        std::atomic<bool> should_stop_{false};
        std::function<void(SharedMemory *)> event_callback_;
        std::chrono::milliseconds poll_interval_;

    public:
        explicit Poller(std::function<void(SharedMemory *)> callback) : event_callback_(std::move(callback)),
                                                                        poll_interval_(std::chrono::milliseconds(20)) {

            reader_ = std::make_unique<SharedMemoryReader>(ProcessResources::FILE_MAPPING_NAME);

        }

        Poller(const char *mapping_name,
               std::function<void(SharedMemory *)> callback,
               std::chrono::milliseconds interval = std::chrono::milliseconds(1000))
                : event_callback_(std::move(callback)), poll_interval_(interval) {

            reader_ = std::make_unique<SharedMemoryReader>(mapping_name);
        }

        ~Poller() {
            Stop();
        }

        Poller(const Poller &) = delete;

        Poller &operator=(const Poller &) = delete;

        bool Start() {
            if (!reader_ || !reader_->IsValid()) {
                return false;
            }

            if (polling_thread_.joinable()) {
                // already running
                return true;
            }

            should_stop_ = false;
            polling_thread_ = std::thread(&Poller::PollThread, this);
            return true;
        }

        void Stop() {
            // if (should_stop_) return;

            should_stop_ = true;

            if (polling_thread_.joinable()) {
                polling_thread_.join();
            }
        }

        bool IsRunning() const {
            return polling_thread_.joinable();
        }

    private:
        void PollThread() {
            auto *sm = reader_->GetSharedMemory();

            auto event1 = reader_->GetEvent1();

            auto event2 = reader_->GetEvent2();

            while (!should_stop_) {
                // Stop() can't be called in DLL_PROCESS_DETACH, since even if the RC is decremented by FreeLibrary,
                // the DLL_PROCESS_DETACH won't be triggered as long as the created thread doesn't exit.
                // This causes a circular dependency, so use a separate event to exit this thread.
                if (WAIT_OBJECT_0 == WaitForSingleObject(reader_->GetExitEvent(), 0)) {
                    should_stop_ = true;
                    return;
                }

                // If dwMilliseconds is zero, the function does not enter a wait state if the object is not signaled;
                // it always returns immediately
                DWORD waitResult = WaitForSingleObject(event1, 0);

                if (waitResult == WAIT_OBJECT_0) {
                    if (event_callback_) {
                        event_callback_(sm);
                    }

                    SetEvent(event2);
                }

                if (WAIT_OBJECT_0 == WaitForSingleObject(reader_->events_.update_imguizmo_flag.get(),
                                                         0)) {
                    if (event_callback_) {
                        event_callback_(sm);
                    }
                }

                std::this_thread::sleep_for(poll_interval_);
            }
        }
    };
}
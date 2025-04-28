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

            while (!should_stop_) {
                DWORD waitResult = WaitForSingleObject(sm->event1, 500);

                if (waitResult == WAIT_OBJECT_0) {
                    if (event_callback_) {
                        event_callback_(sm);
                    }

                    SetEvent(sm->event2);
                }

                std::this_thread::sleep_for(poll_interval_);
            }
        }
    };
}
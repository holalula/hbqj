#pragma once

#include <windows.h>

#include "process_resources.h"

namespace hbqj {
    class SharedMemoryReader {
    private:
        HandleGuard map_file_{nullptr, &SafeCloseHandle};
        MapViewGuard shared_memory_{nullptr, &SafeUnmapViewOfFile};
        bool is_valid_ = false;
        HandleGuard event1_{nullptr, &SafeCloseHandle};
        HandleGuard event2_{nullptr, &SafeCloseHandle};
        HandleGuard exit_event_{nullptr, &SafeCloseHandle};

    public:
        explicit SharedMemoryReader(const char *mapping_name) {
            map_file_.reset(OpenFileMappingA(
                    FILE_MAP_ALL_ACCESS,
                    FALSE,
                    mapping_name
            ));

            if (!IsValidHandle(map_file_.get())) {
                std::cerr << "Failed to open file mapping: " << GetLastError() << std::endl;
                return;
            }

            shared_memory_.reset(MapViewOfFile(
                    map_file_.get(),
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    0
            ));

            if (!shared_memory_.get()) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                return;
            }

            event1_.reset(OpenEventA(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    GetSharedMemory()->event1_name
            ));

            if (!IsValidHandle(event1_.get())) {
                std::cerr << "Invalid event1" << std::endl;
                return;
            }

            event2_.reset(OpenEventA(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    GetSharedMemory()->event2_name
            ));

            if (!IsValidHandle(event2_.get())) {
                std::cerr << "Invalid event2" << std::endl;
                return;
            }

            exit_event_.reset(OpenEventA(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    GetSharedMemory()->exit_event_name
            ));

            is_valid_ = true;
        }

        SharedMemoryReader(const SharedMemoryReader &) = delete;

        SharedMemoryReader &operator=(const SharedMemoryReader &) = delete;

        SharedMemoryReader(SharedMemoryReader &&other) noexcept
                : map_file_(std::move(other.map_file_)),
                  shared_memory_(std::move(other.shared_memory_)),
                  is_valid_(other.is_valid_) {
            other.map_file_ = nullptr;
            other.shared_memory_ = nullptr;
            other.is_valid_ = false;
        }

        SharedMemoryReader &operator=(SharedMemoryReader &&other) noexcept {
            if (this != &other) {
                Cleanup();

                map_file_ = std::move(other.map_file_);
                shared_memory_ = std::move(other.shared_memory_);
                is_valid_ = other.is_valid_;

                other.map_file_ = nullptr;
                other.shared_memory_ = nullptr;
                other.is_valid_ = false;
            }
            return *this;
        }

        ~SharedMemoryReader() {
            Cleanup();
        }

        bool IsValid() const {
            return is_valid_;
        }

        SharedMemory *GetSharedMemory() const {
            return reinterpret_cast<SharedMemory *>(shared_memory_.get());
        }

        HANDLE GetEvent1() const { return event1_.get(); }

        HANDLE GetEvent2() const { return event2_.get(); }

        HANDLE GetExitEvent() const { return exit_event_.get(); }

    private:
        void Cleanup() {
            is_valid_ = false;
        }
    };
}
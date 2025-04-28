#pragma once

#include <windows.h>

#include "process_resources.h"

namespace hbqj {
    class SharedMemoryReader {
    private:
        HANDLE map_file_ = nullptr;
        SharedMemory *shared_memory_ = nullptr;
        bool is_valid_ = false;
        HANDLE event1_ = nullptr;
        HANDLE event2_ = nullptr;

    public:
        explicit SharedMemoryReader(const char *mapping_name) {
            map_file_ = OpenFileMapping(
                    FILE_MAP_ALL_ACCESS,
                    FALSE,
                    mapping_name
            );

            if (!map_file_) {
                std::cerr << "Failed to open file mapping: " << GetLastError() << std::endl;
                return;
            }

            shared_memory_ = reinterpret_cast<SharedMemory *>(MapViewOfFile(
                    map_file_,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    0
            ));

            if (!shared_memory_) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                CloseHandle(map_file_);
                map_file_ = nullptr;
                return;
            }

            event1_ = OpenEvent(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    shared_memory_->event1_name
            );

            if (event1_ == nullptr || event1_ == INVALID_HANDLE_VALUE) {
                std::cerr << "Invalid event1" << std::endl;
                UnmapViewOfFile(shared_memory_);
                CloseHandle(map_file_);
                shared_memory_ = nullptr;
                map_file_ = nullptr;
                event1_ = nullptr;
                return;
            }

            event2_ = OpenEvent(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    shared_memory_->event2_name
            );

            if (event2_ == nullptr || event2_ == INVALID_HANDLE_VALUE) {
                std::cerr << "Invalid event2" << std::endl;
                UnmapViewOfFile(shared_memory_);
                CloseHandle(map_file_);
                shared_memory_ = nullptr;
                map_file_ = nullptr;
                event1_ = nullptr;
                event2_ = nullptr;
                return;
            }

            is_valid_ = true;
        }

        SharedMemoryReader(const SharedMemoryReader &) = delete;

        SharedMemoryReader &operator=(const SharedMemoryReader &) = delete;

        SharedMemoryReader(SharedMemoryReader &&other) noexcept
                : map_file_(other.map_file_),
                  shared_memory_(other.shared_memory_),
                  is_valid_(other.is_valid_) {
            other.map_file_ = nullptr;
            other.shared_memory_ = nullptr;
            other.is_valid_ = false;
        }

        SharedMemoryReader &operator=(SharedMemoryReader &&other) noexcept {
            if (this != &other) {
                Cleanup();

                map_file_ = other.map_file_;
                shared_memory_ = other.shared_memory_;
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
            return shared_memory_;
        }

        HANDLE GetEvent1() const { return event1_; }

        HANDLE GetEvent2() const { return event2_; }

    private:
        void Cleanup() {
            if (shared_memory_) {
                UnmapViewOfFile(shared_memory_);
                shared_memory_ = nullptr;
            }

            if (map_file_) {
                CloseHandle(map_file_);
                map_file_ = nullptr;
            }

            if (event1_) {
                CloseHandle(event1_);
                event1_ = nullptr;
            }

            if (event2_) {
                CloseHandle(event2_);
                event2_ = nullptr;
            }

            is_valid_ = false;
        }
    };
}
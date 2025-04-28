#pragma once

#include <iostream>
#include <Windows.h>
#include <synchapi.h>

namespace hbqj {

#pragma pack(push, 1)
    struct SharedMemory {
        int data1;
        HANDLE event1;
        int data2;
        HANDLE event2;
    };
#pragma pack(pop)

    class ProcessResources {
    public:
        ProcessResources(const ProcessResources &) = delete;

        ProcessResources &operator=(const ProcessResources &) = delete;

        ProcessResources(ProcessResources &&) = delete;

        ProcessResources &operator=(ProcessResources &&) = delete;

        static ProcessResources &GetInstance() {
            static ProcessResources instance;
            return instance;
        }

        SharedMemory *GetSharedMemory() const {
            return shared_memory_;
        }

        bool IsValid() const {
            return shared_memory_ != nullptr &&
                   map_file_ != nullptr &&
                   shared_memory_->event1 != nullptr &&
                   shared_memory_->event2 != nullptr;
        }

    private:
        ProcessResources() {
            Initialize();
        }


        ~ProcessResources() {
            cleanup();
        }

        HANDLE map_file_ = nullptr;
        SharedMemory *shared_memory_ = nullptr;
        PSECURITY_DESCRIPTOR security_descriptor_ = nullptr;

        static SECURITY_ATTRIBUTES CreateEveryoneAccessSecurity() {
            SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES)};
            ConvertStringSecurityDescriptorToSecurityDescriptorA(
                    "D:(A;;GA;;;WD)", // allow all users access
                    SDDL_REVISION_1,
                    &sa.lpSecurityDescriptor,
                    nullptr);
            return sa;
        }

        void Initialize() {
            SECURITY_ATTRIBUTES sa = CreateEveryoneAccessSecurity();
            security_descriptor_ = sa.lpSecurityDescriptor;

            map_file_ = CreateFileMapping(
                    INVALID_HANDLE_VALUE,
                    &sa,
                    PAGE_READWRITE,
                    0,
                    sizeof(SharedMemory),
                    "Local\\HBQJSM");

            if (!map_file_) {
                std::cerr << "Failed to create file mapping: " << GetLastError() << std::endl;
                LocalFree(security_descriptor_);
                security_descriptor_ = nullptr;
                return;
            }

            shared_memory_ = reinterpret_cast<SharedMemory *>(MapViewOfFile(
                    map_file_,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    0));

            if (!shared_memory_) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                CloseHandle(map_file_);
                map_file_ = nullptr;
                LocalFree(security_descriptor_);
                security_descriptor_ = nullptr;
                return;
            }

            ZeroMemory(shared_memory_, sizeof(SharedMemory));

            shared_memory_->event1 = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    "Global\\HBQJ_Event1");

            if (!shared_memory_->event1) {
                std::cerr << "Failed to create event1: " << GetLastError() << std::endl;
                cleanup();
                return;
            }

            shared_memory_->event2 = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    "Global\\HBQJ_Event2");

            if (!shared_memory_->event2) {
                std::cerr << "Failed to create event2: " << GetLastError() << std::endl;
                cleanup();
                return;
            }

            std::cout << "Process resources initialized successfully" << std::endl;
        }

        void cleanup() {
            if (shared_memory_) {
                if (shared_memory_->event1) {
                    CloseHandle(shared_memory_->event1);
                    shared_memory_->event1 = nullptr;
                }

                if (shared_memory_->event2) {
                    CloseHandle(shared_memory_->event2);
                    shared_memory_->event2 = nullptr;
                }

                UnmapViewOfFile(shared_memory_);
                shared_memory_ = nullptr;
            }

            if (map_file_) {
                CloseHandle(map_file_);
                map_file_ = nullptr;
            }

            if (security_descriptor_) {
                LocalFree(security_descriptor_);
                security_descriptor_ = nullptr;
            }

            std::cout << "Process resources cleaned up" << std::endl;
        }
    };
}

#pragma once

#include <iostream>
#include <Windows.h>
#include <synchapi.h>

namespace hbqj {

#pragma pack(push, 1)
    struct SharedMemory {
        int data1;
        char event1_name[64];
        int data2;
        char event2_name[64];
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
                   event1_ != nullptr &&
                   event2_ != nullptr;
        }

        HANDLE GetEvent1() const { return event1_; };

        HANDLE GetEvent2() const { return event2_; };

        static std::string GetEventName(int index) {
            return std::format("Local\\HBQJ_Event_{}", index);
        }

        static constexpr const char *FILE_MAPPING_NAME = "Local\\HBQJSM";

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
        HANDLE event1_ = nullptr;
        HANDLE event2_ = nullptr;

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
                    FILE_MAPPING_NAME);

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

            event1_ = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(1).c_str());

            if (!event1_) {
                std::cerr << "Failed to create event1: " << GetLastError() << std::endl;
                cleanup();
                return;
            }

            event2_ = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(2).c_str());

            if (!event2_) {
                std::cerr << "Failed to create event2: " << GetLastError() << std::endl;
                cleanup();
                return;
            }

            strcpy_s(shared_memory_->event1_name, GetEventName(1).c_str());
            strcpy_s(shared_memory_->event2_name, GetEventName(2).c_str());

            std::cout << "Process resources initialized successfully" << std::endl;
        }

        void cleanup() {
            if (shared_memory_) {
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

            if (event1_) {
                CloseHandle(event1_);
                event1_ = nullptr;
            }

            if (event2_) {
                CloseHandle(event2_);
                event2_ = nullptr;
            }

            std::cout << "Process resources cleaned up" << std::endl;
        }
    };
}

#pragma once

#include <iostream>
#include <windows.h>
#include <synchapi.h>
#include <sddl.h>

namespace hbqj {

    static void SafeCloseHandle(HANDLE handle) {
        // https://stackoverflow.com/questions/47575594/is-it-safe-to-call-closehandle-handle-who-handle-is-null
        if (handle && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }

    static bool IsValidHandle(HANDLE handle) {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    static void SafeLocalFree(PSECURITY_DESCRIPTOR sd) {
        if (sd) {
            LocalFree(sd);
        }
    }

    static void SafeUnmapViewOfFile(void *p) {
        if (p) {
            UnmapViewOfFile(p);
        }
    }

    using HandleGuard = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&SafeCloseHandle)>;

    using SecurityDescriptorGuard = std::unique_ptr<std::remove_pointer_t<PSECURITY_DESCRIPTOR>, decltype(&SafeLocalFree)>;

    using MapViewGuard = std::unique_ptr<void, decltype(&SafeUnmapViewOfFile)>;

#pragma pack(push, 1)
    struct SharedMemory {
        int data1;
        char event1_name[64];
        int data2;
        char event2_name[64];
        char exit_event_name[64];
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
            return reinterpret_cast<SharedMemory *>(shared_memory_.get());
        }

        bool IsValid() const {
            return shared_memory_.get() != nullptr &&
                   map_file_.get() != nullptr &&
                   event1_.get() != nullptr &&
                   event2_.get() != nullptr;
        }

        HANDLE GetEvent1() const { return event1_.get(); };

        HANDLE GetEvent2() const { return event2_.get(); };

        HANDLE GetExitEvent() const { return exit_event_.get(); }

        static std::string GetEventName(int index) {
            return std::format("Local\\HBQJ_Event_{}", index);
        }

        static constexpr const char *FILE_MAPPING_NAME = "Local\\HBQJSM";

    private:
        ProcessResources() {
            Initialize();
        }


        ~ProcessResources() = default;

        HandleGuard map_file_{nullptr, &SafeCloseHandle};
        MapViewGuard shared_memory_{nullptr, &SafeUnmapViewOfFile};
        SecurityDescriptorGuard security_descriptor_{nullptr, &SafeLocalFree};
        HandleGuard event1_{nullptr, &SafeCloseHandle};
        HandleGuard event2_{nullptr, &SafeCloseHandle};
        HandleGuard exit_event_{nullptr, &SafeCloseHandle};

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
            security_descriptor_.reset(sa.lpSecurityDescriptor);

            map_file_.reset(CreateFileMappingA(
                    INVALID_HANDLE_VALUE,
                    &sa,
                    PAGE_READWRITE,
                    0,
                    sizeof(SharedMemory),
                    FILE_MAPPING_NAME));

            if (!IsValidHandle(map_file_.get())) {
                std::cerr << "Failed to create file mapping: " << GetLastError() << std::endl;
                return;
            }

            shared_memory_.reset(MapViewOfFile(
                    map_file_.get(),
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    0));

            if (!shared_memory_.get()) {
                std::cerr << "Failed to map view of file: " << GetLastError() << std::endl;
                return;
            }

            ZeroMemory(shared_memory_.get(), sizeof(SharedMemory));

            event1_.reset(CreateEventA(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(1).c_str()));

            if (!IsValidHandle(event1_.get())) {
                std::cerr << "Failed to create event1: " << GetLastError() << std::endl;
                return;
            }

            event2_.reset(CreateEventA(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(2).c_str()));

            if (!IsValidHandle(event2_.get())) {
                std::cerr << "Failed to create event2: " << GetLastError() << std::endl;
                return;
            }

            exit_event_.reset(CreateEventA(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(3).c_str()
            ));

            strcpy_s(GetSharedMemory()->event1_name, GetEventName(1).c_str());
            strcpy_s(GetSharedMemory()->event2_name, GetEventName(2).c_str());
            strcpy_s(GetSharedMemory()->exit_event_name, GetEventName(3).c_str());

            std::cout << "Process resources initialized successfully" << std::endl;
        }
    };
}

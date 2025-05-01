#pragma once

#include <iostream>
#include <windows.h>
#include <synchapi.h>
#include <sddl.h>

#include "event_manager.h"

namespace hbqj {
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

        HbqjEvents events_{};

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

            if (auto result = EventManager::CreateHbqjEvent(EventType::UpdateImGuizmoFlag)) {
                events_.update_imguizmo_flag = std::move(result.value());
            }

            std::cout << "Process resources initialized successfully" << std::endl;
        }
    };
}

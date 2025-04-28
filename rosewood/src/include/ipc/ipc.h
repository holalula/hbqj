#pragma once

#include <windows.h>
#include <sddl.h>
#include <string>

#include "process_resources.h"

namespace hbqj {
    class __declspec(dllexport) Ipc {
    public:
        static void Test() {
            auto& resources = ProcessResources::GetInstance();

            if (!resources.IsValid()) {
                std::cerr << "Process resources are not valid" << std::endl;
                return;
            }

            auto* sm = resources.GetSharedMemory();

            sm->data1 = 1;
            sm->data2 = 0;
            SetEvent(sm->event1);

            // create a reader thread
            HANDLE reader_thread = CreateThread(
                    nullptr,
                    0,
                    reinterpret_cast<LPTHREAD_START_ROUTINE>(ReaderProcess),
                    nullptr,
                    0,
                    nullptr
            );

            auto wait_result = WaitForSingleObject(sm->event2, 5000);

            if (wait_result != WAIT_OBJECT_0) {
                std::cout << "Timeout after 5s." << std::endl;
            } else {
                std::cout << "Get Data in Writer:" << std::endl;
                std::cout << sm->data2 << std::endl;
            }

            WaitForSingleObject(reader_thread, INFINITE);

            CloseHandle(reader_thread);
        };

        static void ReaderProcess() {
            HANDLE map_file = OpenFileMapping(
                    FILE_MAP_ALL_ACCESS,
                    FALSE,
                    "Local\\HBQJSM"
            );

            if (!map_file) {
                std::cout << "Failed to open file mapping." << std::endl;
                return;
            }

            auto sm = reinterpret_cast<SharedMemory *>(MapViewOfFile(
                    map_file,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    0
            ));

            if (!sm) {
                CloseHandle(map_file);
                return;
            }

            if (sm->event1 == nullptr || sm->event1 == INVALID_HANDLE_VALUE) {
                std::cout << "Invalid event1" << std::endl;
                UnmapViewOfFile(sm);
                CloseHandle(map_file);
                return;
            }

            if (sm->event2 == nullptr || sm->event2 == INVALID_HANDLE_VALUE) {
                std::cout << "Invalid event2" << std::endl;
                UnmapViewOfFile(sm);
                CloseHandle(map_file);
                return;
            }

            auto wait_result = WaitForSingleObject(sm->event1, 5000);
            if (wait_result == WAIT_OBJECT_0) {
                sm->data2 = 20;

                SetEvent(sm->event2);
            }

            UnmapViewOfFile(sm);
            CloseHandle(map_file);
        }
    };
}
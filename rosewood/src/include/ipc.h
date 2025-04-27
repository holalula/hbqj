#pragma once

#include <windows.h>
#include <sddl.h>
#include <synchapi.h>
#include <string>

namespace hbqj {

#pragma pack(push, 1)
    struct SharedMemory {
        int data1;
        HANDLE event1;
        int data2;
        HANDLE event2;
    };
#pragma pack(pop)

    class __declspec(dllexport) Ipc {
    public:
        static SECURITY_ATTRIBUTES CreateEveryoneAccessSecurity() {
            SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES)};
            ConvertStringSecurityDescriptorToSecurityDescriptorA(
                    "D:(A;;GA;;;WD)", // allow all users access
                    SDDL_REVISION_1,
                    &sa.lpSecurityDescriptor,
                    nullptr);
            return sa;
        };

        static void CreateSharedMemory() {
            SECURITY_ATTRIBUTES sa = CreateEveryoneAccessSecurity();

            HANDLE map_file = CreateFileMapping(
                    // INVALID_HANDLE_VALUE means creating a file mapping object of a specified size
                    // that is backed by the system paging file instead of by a file in the file system
                    INVALID_HANDLE_VALUE,
                    &sa,
                    PAGE_READWRITE,
                    0,
                    sizeof(SharedMemory),
                    "Local\\HBQJSM");

            if (!map_file) {
                LocalFree(sa.lpSecurityDescriptor);
                return;
            }

            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                std::cout << "File mapping already exists, reuse it.." << std::endl;
                // CloseHandle(map_file);
                // LocalFree(sa.lpSecurityDescriptor);
                // return;
            }

            auto *sm = reinterpret_cast<SharedMemory *>(MapViewOfFile(
                    map_file,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    // zero means the mapping extends from the specified offset to the end of the file mapping
                    0)
            );

            if (!sm) {
                CloseHandle(map_file);
                LocalFree(sa.lpSecurityDescriptor);
                return;
            }

            ZeroMemory(sm, sizeof(SharedMemory));

            sm->event1 = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    "event_1"
            );

            if (!sm->event1) {
                UnmapViewOfFile(sm);
                CloseHandle(map_file);
                LocalFree(sa.lpSecurityDescriptor);
                return;
            }

            sm->event2 = CreateEvent(
                    &sa,
                    FALSE,
                    FALSE,
                    "event_2"
            );

            if (!sm->event2) {
                CloseHandle(sm->event1);
                UnmapViewOfFile(sm);
                CloseHandle(map_file);
                LocalFree(sa.lpSecurityDescriptor);
                return;
            }

            LocalFree(sa.lpSecurityDescriptor);

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

            // clean up resources
            CloseHandle(reader_thread);
            CloseHandle(sm->event1);
            CloseHandle(sm->event2);
            UnmapViewOfFile(sm);
            CloseHandle(map_file);
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
#pragma once

#include <Windows.h>
#include <sddl.h>
#include <string>

#include "ipc/process_resources.h"
#include "ipc/poller.h"

namespace hbqj {

    // event callback function
    void OnEvent(SharedMemory *sm) {
        sm->data2 = 420;
    }

    class __declspec(dllexport) Ipc {
    public:
        static void Test() {
            auto &resources = ProcessResources::GetInstance();

            if (!resources.IsValid()) {
                std::cerr << "Process resources are not valid" << std::endl;
                return;
            }

            auto *sm = resources.GetSharedMemory();

            sm->data1 = 1;
            sm->data2 = 0;
            SetEvent(sm->event1);

            auto poller = std::make_unique<Poller>(
                    "Local\\HBQJSM",
                    OnEvent,
                    std::chrono::milliseconds(500)
            );

            if (poller->Start()) {
                std::cout << "Start Poller Thread.." << std::endl;
            } else {
                std::cout << "Failed to Start Poller Thread.." << std::endl;
            }

            auto wait_result = WaitForSingleObject(sm->event2, 5000);

            if (wait_result != WAIT_OBJECT_0) {
                std::cout << "Timeout after 5s." << std::endl;
            } else {
                std::cout << "Get Data in Writer:" << std::endl;
                std::cout << sm->data2 << std::endl;
            }

            if (poller) {
                poller->Stop();
                std::cout << "Poller Thread Exits." << std::endl;
            }
        };
    };
}
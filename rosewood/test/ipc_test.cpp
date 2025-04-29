#include <gtest/gtest.h>
#include <iostream>

#include "ipc/ipc.h"
#include "hook.h"

namespace hbqj {
    static const char *dll_name = "namazu.dll";

    TEST(IpcTest, Example) {
        std::cout << "IPC" << std::endl;
        Ipc::Test();
    }

    TEST(IpcTest, Inject) {
        auto &resources = ProcessResources::GetInstance();

        if (!resources.IsValid()) {
            std::cerr << "Process resources are not valid" << std::endl;
            return;
        }

        auto *sm = resources.GetSharedMemory();

        sm->data1 = 1;
        sm->data2 = 0;
        SetEvent(resources.GetEvent1());


        // Inject and start poller in game process
        Hook hook;

        const auto &dir = std::filesystem::current_path();
        const auto &dll_path =
                dir.parent_path().parent_path().parent_path() / "x64" / "Release" / dll_name;
        hook.log.info("current dir: {}", dir.string());
        hook.log.info("dll path: {}", utf16_to_utf8(dll_path.wstring()));

        const auto &result = hook.Inject(dll_path.wstring(), "ffxiv_dx11.exe");
        if (!result) {
            hook.log.error("{}", result.error());
        }

        // Wait for event from poller
        auto wait_result = WaitForSingleObject(resources.GetEvent2(), 50000);

        if (wait_result != WAIT_OBJECT_0) {
            std::cout << "Timeout after 5s." << std::endl;
        } else {
            std::cout << "Get data in writer thread:" << std::endl;
            std::cout << sm->data2 << std::endl;
        }
    }

    TEST(IpcTest, Unload) {
        auto &resources = ProcessResources::GetInstance();

        if (!resources.IsValid()) {
            std::cerr << "Process resources are not valid" << std::endl;
            return;
        }

        SetEvent(resources.GetExitEvent());

        Hook hook;

        auto result = hook.Unload(L"namazu.dll", "ffxiv_dx11.exe");
        if (!result) {
            hook.log.error("{}", result.error());
        } else if (result && result.value()) {
            std::cout << "Unload successfully" << std::endl;
        }
    }
}
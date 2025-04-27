#include <gtest/gtest.h>
#include <iostream>

#include "ipc.h"

namespace hbqj {
    TEST(IpcTest, Example) {
        std::cout << "IPC" << std::endl;
        Ipc::CreateSharedMemory();
    }
}
#include <gtest/gtest.h>

#include "ipc/heart_beat.h"

namespace hbqj {
    TEST(HeartBeatTest, Test) {
        auto& hb = HeartBeatMonitor::GetInstance();

        hb.Start();

        std::this_thread::sleep_for(std::chrono::seconds(10));

        hb.Stop();
    }
}
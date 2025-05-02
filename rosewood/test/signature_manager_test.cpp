#include <gtest/gtest.h>
#include <iostream>

#include "utils/scoped_timer.h"
#include "signature_manager.h"
#include "signature_scanner.h"

namespace hbqj {
    TEST(SignatureManagerTest, Example) {
        // GTEST_SKIP();
        SignatureManager manager_;
        auto process = std::make_shared<Process>("ffxiv_dx11.exe", "ffxiv_dx11.exe");

        {
            MEASURE_TIME("scan all signatures.");
            manager_.Initialize(process);
        }

        auto result = manager_.GetSignature(SignatureType::BaseHouse);
        EXPECT_TRUE(result.has_value());

        auto view_matrix_offset = process->GetOffsetAddr(
                manager_.GetSignature(SignatureType::ViewMatrix).value()->addr);
        process->log.info("ViewMatrix offset: 0x{:x}", process->CalculateTargetOffsetCall(view_matrix_offset).value());

        auto select_housing_item = process->GetOffsetAddr(
                manager_.GetSignature(SignatureType::SelectHousingItem).value()->addr);
        process->log.info("Select offset: 0x{:x}", select_housing_item);

        auto place_housing_item = process->GetOffsetAddr(
                manager_.GetSignature(SignatureType::PlaceHousingItem).value()->addr);
        process->log.info("Place offset: 0x{:x}", place_housing_item);
    }

    TEST(SignatureManagerTest, SignatureNotFound) {
        GTEST_SKIP();
        SignatureManager m_;
        m_.GetSignature(SignatureType::ActorTable);
    }

    TEST(SignatureManagerTest, GetActiveCamera) {
        auto process = std::make_shared<Process>("ffxiv_dx11.exe", "ffxiv_dx11.exe");

        SignatureManager manager_;
        manager_.Initialize(process);

        auto offset = manager_.GetSignature(SignatureType::GetActiveCamera)
                .transform([](auto sig) { return sig->addr; })
                .transform([&process](auto addr) { return process->GetOffsetAddr(addr); })
                .and_then([&process](auto addr) { return process->CalculateTargetOffsetCall(addr); });

        if (offset) {
            Logger::Info("GetActionCameraOffset: 0x{:x}", offset.value());
        }
    }
}
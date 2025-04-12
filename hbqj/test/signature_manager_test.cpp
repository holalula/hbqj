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
	}

	TEST(SignatureManagerTest, SignatureNotFound) {
		GTEST_SKIP();
		SignatureManager m_;
		m_.GetSignature(SignatureType::ActorTable);
	}
}
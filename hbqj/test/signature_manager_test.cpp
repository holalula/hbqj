#include <gtest/gtest.h>

#include "signature_manager.h"
#include "signature_scanner.h"

namespace hbqj {
	TEST(SignatureManagerTest, Example) {
		GTEST_SKIP();
		SignatureManager manager_;
		manager_.Initialize();
		auto result = manager_.GetSignature(SignatureType::BaseHouse);
		EXPECT_TRUE(result.has_value());
	}

	TEST(SignatureManagerTest, SignatureNotFound) {
		GTEST_SKIP();
		SignatureManager m_;
		m_.GetSignature(SignatureType::ActorTable);
	}
}
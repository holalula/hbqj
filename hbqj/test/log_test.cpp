#include <gtest/gtest.h>

#include "log.h"

namespace hbqj {
	TEST(LogTest, LogExample) {
		GTEST_SKIP();
		auto log = Logger::GetLogger("LogExample");

		log.info("Info message: {}", 1);
		log.warn("Warn message: {}", 2);
		log.error("Error message: {}", 3);

		log.info("Hex output: 0x{:x}, 0x{:x}, {:x}", 1234, 0x1234, 0);
	}
}

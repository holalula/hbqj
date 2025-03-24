#include <gtest/gtest.h>
#include <memory.h>

#include "memory.h"

namespace hbqj {
	TEST(MemoryTest, Example) {
		auto process = std::make_shared<Process>();
		Memory memory;
		memory.Initialize(process);
		memory.PlaceAnywhere(true);
	}
}
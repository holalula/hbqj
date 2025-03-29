#include <gtest/gtest.h>
#include <memory.h>
#include <iostream>

#include "memory.h"

namespace hbqj {
	TEST(MemoryTest, GetActivePositionAndRotationExample) {
		GTEST_SKIP();
		auto process = std::make_shared<Process>();
		Memory memory;
		memory.Initialize(process);
		auto position_result = memory.GetActivePosition();
		if (position_result) {
			auto position = position_result.value();
			process->log.info("Position: {}, {}, {}", position.x, position.y, position.z);
		}
		else {
			process->log.error("{}", position_result.error());
		}

		auto rotation_result = memory.GetActiveRotation();
		if (rotation_result) {
			auto rotation = rotation_result.value();
			process->log.info("Rotation: {}, {}, {}, {}", rotation.x, rotation.y, rotation.z, rotation.w);
		}
		else {
			process->log.error("{}", rotation_result.error());
		}
	}

	TEST(MemoryTest, SetActivePositionExample) {
		GTEST_SKIP();
		auto process = std::make_shared<Process>();
		Memory memory;
		memory.Initialize(process);
		if (memory.SetActivePosition(1., 1., 1.)) {
			process->log.info("Updated.");
		}
	}

	TEST(MemoryTest, GetFurnitureListExample) {
		GTEST_SKIP();
		auto process = std::make_shared<Process>();
		Memory memory;
		memory.Initialize(process);
		auto result = memory.GetFurnitureList();
		if (result.has_value()) {
			for (const auto& item : result.value()) {
				process->log.info("{}", item);
			}
		}
		else {
			process->log.error("{}", result.error());
		}
	}

	TEST(MemoryTest, JsonExample) {
		Position pos = { .x = 1.1, .y = 2.2, .z = 3.3 };
		json j = pos;
		std::cout << j << std::endl;

		std::string pos_json_str = j.dump(2);
		std::cout << pos_json_str << std::endl;

		std::string json_input_str = R"({"x":1.100000023841858,"y":2.200000047683716,"z":3.299999952316284})";
		Position parsed_pos = json::parse(json_input_str).get<Position>();
		std::cout << "x = " << parsed_pos.x << std::endl;
	}
}
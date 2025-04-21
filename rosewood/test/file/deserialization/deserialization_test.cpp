#include <gtest/gtest.h>
#include <iostream>

#include "file.h"
#include "file/deserialization/hbqj_deserializer.h"

namespace hbqj {
    TEST(DeserializationTest, HBQJ_Without_Color) {
        const auto &file = File::ReadFile(std::filesystem::current_path() / "hbqj.json");
        IDeserializer<FurnitureLayout> deserializer{};

        FurnitureLayout furnitureLayout;
        if (deserializer.tryDeserialize(file, furnitureLayout)) {
            std::cout << std::format("{}", furnitureLayout) << std::endl;
            std::cout << "total size: " << furnitureLayout.size << std::endl;
        } else {
            std::cout << "Failed to deserialize file.";
        }
    }

    TEST(DeserializationTest, HBQJ) {
        const auto &file = File::ReadFile(std::filesystem::current_path() / "hbqj_w_color.json");
        IDeserializer<FurnitureLayout> deserializer{};

        FurnitureLayout furnitureLayout;
        if (deserializer.tryDeserialize(file, furnitureLayout)) {
            std::cout << std::format("{}", furnitureLayout) << std::endl;
            std::cout << "total size: " << furnitureLayout.size << std::endl;
        } else {
            std::cout << "Failed to deserialize file.";
        }
    }
}
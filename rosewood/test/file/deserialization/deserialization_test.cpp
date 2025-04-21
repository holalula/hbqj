#include <gtest/gtest.h>
#include <iostream>

#include "file.h"
#include "file/deserialization/legacy_deserializer.h"
#include "file/deserialization/hbqj_deserializer.h"

namespace hbqj {
    TEST(DeserializationTest, Legacy_Without_Color) {
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

    TEST(DeserializationTest, Legacy) {
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

    TEST(DeserializationTest, Hbqj) {
        const auto &file = File::ReadFile(std::filesystem::current_path() / "housing");
        IDeserializer<HousingLayout> deserializer{};

        HousingLayout housing;
        // TODO: handle json with invalid format
        if (deserializer.tryDeserialize(file, housing)) {
            std::cout << std::format("{}", housing) << std::endl;
        } else {
            std::cout << "Failed to deserialize file.";
        }
    }
}
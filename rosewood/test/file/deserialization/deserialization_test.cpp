#include <gtest/gtest.h>
#include <iostream>

#include "file.h"
#include "file/struct/legacy.h"
#include "file/struct/hbqj.h"
#include "file/deserialization/deserializer.h"

namespace hbqj {
    TEST(DeserializationTest, Legacy) {
        std::vector<std::string> files{"hbqj.json", "hbqj_w_color.json"};

        for (const auto &file_name: files) {

            const auto &file = File::ReadFile(std::filesystem::current_path() / file_name);

            if (const auto &result = Deserializer::Deserialize<FurnitureLayout>(file)) {
                std::cout << std::format("{}", result.value()) << std::endl;
                std::cout << "total size: " << result.value().size << std::endl;
            } else {
                std::cout << "Failed to deserialize file.";
            }

        }
    }

    TEST(DeserializationTest, Hbqj) {
        const auto &file = File::ReadFile(std::filesystem::current_path() / "housing");

        // TODO: handle json with invalid format
        if (const auto &result = Deserializer::Deserialize<HousingLayout>(file)) {
            std::cout << std::format("{}", result.value()) << std::endl;
        } else {
            std::cout << "Failed to deserialize file.";
        }
    }
}
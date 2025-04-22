#include <gtest/gtest.h>
#include <filesystem>
#include <format>

#include "file/decryption/aes_decryptor.h"
#include "file/file_reader.h"
#include "file/struct/hbqj.h"
#include "file/struct/legacy.h"

namespace hbqj {
    TEST(FileReaderTest, Hbqj) {
        const auto &path = std::filesystem::current_path() / "housing";

        FileReader reader;
        reader.AddDecryptionHandler(std::make_unique<AesDecryptor>());

        reader.RegisterDeserializer<FurnitureLayout>();
        reader.RegisterDeserializer<HousingLayout>();

        auto result = reader.ReadFile<HousingLayout>(path);
        if (result) {
            std::cout << std::format("{}", result.value());
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }

    TEST(FileReaderTest, Legacy) {
        const auto &path = std::filesystem::current_path() / "hbqj.json";

        FileReader reader;
        reader.AddDecryptionHandler(std::make_unique<AesDecryptor>());

        reader.RegisterDeserializer<FurnitureLayout>();
        reader.RegisterDeserializer<HousingLayout>();

        auto result = reader.ReadFile<FurnitureLayout>(path);
        if (result) {
            std::cout << std::format("{}", result.value());
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }

    TEST(FileReaderTest, LegacyWithColor) {
        const auto &path = std::filesystem::current_path() / "hbqj_w_color.json";

        FileReader reader;
        reader.AddDecryptionHandler(std::make_unique<AesDecryptor>());

        reader.RegisterDeserializer<FurnitureLayout>();
        reader.RegisterDeserializer<HousingLayout>();

        auto result = reader.ReadFile<FurnitureLayout>(path);
        if (result) {
            std::cout << std::format("{}", result.value());
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }

    TEST(FileReaderTest, Encrypted) {
        const auto &path = std::filesystem::current_path() / "encrypted";

        FileReader reader;
        reader.AddDecryptionHandler(std::make_unique<AesDecryptor>());

        reader.RegisterDeserializer<FurnitureLayout>();
        reader.RegisterDeserializer<HousingLayout>();

        auto result = reader.ReadFile<FurnitureLayout>(path);
        if (result) {
            std::cout << std::format("{}", result.value());
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }
}
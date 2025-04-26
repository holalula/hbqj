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

    TEST(FileReaderTest, AutoDetectType) {
        // const char* file_name = "encrypted";
        // const char* file_name = "hbqj.json";
        // const char* file_name = "hbqj_w_color.json";
        const char *file_name = "housing";
        const auto &path = std::filesystem::current_path() / file_name;

        FileReader reader;
        reader.AddDecryptionHandler(std::make_unique<AesDecryptor>());

        reader.RegisterDeserializer<FurnitureLayout>();
        reader.RegisterDeserializer<HousingLayout>();

        auto result = reader.ReadFileAutoDetect(path);
        if (result) {
            std::cout << "Detected type: " << result->type_name << std::endl;

            if (result->type_name == typeid(HousingLayout).name()) {
                const auto &layout = std::any_cast<HousingLayout>(result->data);
                std::cout << std::format("{}", layout);
            } else if (result->type_name == typeid(FurnitureLayout).name()) {
                const auto &layout = std::any_cast<FurnitureLayout>(result->data);
                std::cout << std::format("{}", layout);
            }
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }

    TEST(FileReaderTest, GetHousingLayoutFromFile) {
        // const char *file_name = "encrypted";
        // const char* file_name = "hbqj.json";
        // const char* file_name = "hbqj_w_color.json";
        const char *file_name = "housing";
        const auto &path = std::filesystem::current_path() / file_name;

        FileReader reader;

        auto result = reader.ReadFileAutoDetect(path);

        if (result) {
            const auto &layout = FileReader::ToHousingLayout(result.value());
            std::cout << std::format("{}", layout);
        } else {
            std::cout << "Failed to read file." << std::endl;
        }
    }
}
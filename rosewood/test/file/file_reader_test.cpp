#include <gtest/gtest.h>
#include <filesystem>
#include <format>

#include "file/decryption/aes_decryptor.h"
#include "file/file_reader.h"

namespace hbqj {
    TEST(FileReaderTest, GetHousingLayoutFromFile) {
        std::vector<std::string> files{"encrypted", "hbqj.json", "hbqj_w_color.json", "housing"};

        for (const auto &file: files) {
            const auto &path = std::filesystem::current_path() / file;
            FileReader reader;

            auto result = reader.DeserializeFile(path);

            if (result) {
                const auto &layout = FileReader::ToHousingLayout(result.value());
                std::cout << std::format("file: {}, size = {}", file, layout.items.size()) << std::endl;
            } else {
                std::cout << "Failed to read file: " << file << std::endl;
            }
        }
    }
}
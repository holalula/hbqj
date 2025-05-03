#include <gtest/gtest.h>

#include "file/csv/reader.h"
#include "log.h"

namespace hbqj {
    TEST(CsvReaderTest, Furniture) {
        const auto &file_path = std::filesystem::current_path() / "HousingFurniture.csv";
        CsvParser::Parse(file_path);
    }

    TEST(CsvReaderTest, ParseCsvLine) {
        const auto &file_path = std::filesystem::current_path() / "HousingFurniture.csv";

        std::ifstream file(file_path);

        std::string line;

        for (int i = 0; i < 15; i++) {
            std::getline(file, line);

            std::cout << line << std::endl;

            for (const auto &parsed: CsvParser::ParseCsvLine(line)) {
                std::cout << parsed << std::endl;
            }
        }
    }

    TEST(CsvReaderTest, PreviewableFurniture) {
        const auto &file_path = std::filesystem::current_path() / "HousingFurniture.csv";

        const auto &previewable_items = CsvParser::PreviewableFurnitureList(file_path);
        const auto &key_to_name = CsvParser::FurnitureKeyToNameMapping(file_path);

        for (auto const [key, name]: key_to_name) {
            if (std::find(previewable_items.begin(), previewable_items.end(), key) == previewable_items.end()) {
                Logger::Info("{}", name);
            }
        }
    }
}
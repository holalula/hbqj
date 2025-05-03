#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <tuple>
#include <unordered_map>
#include <ranges>

namespace hbqj {
    struct HousingFurnitureCsv {
        int key;
        std::string custom_talk;
        std::string name;
    };

    class CsvParser {
    public:
        static auto ParseCsvLine(const std::string &line) {
            std::vector<std::string> result;
            std::stringstream ss(line);
            std::string item;

            bool in_quotes = false;
            std::string current_field;

            for (char ch: line) {
                if (ch == '"') {
                    in_quotes = !in_quotes;
                } else if (ch == ',' && !in_quotes) {
                    result.push_back(current_field);
                    current_field.clear();
                } else {
                    current_field += ch;
                }
            }

            result.push_back(current_field);

            return result;
        }

        static std::vector<HousingFurnitureCsv> Parse(const std::filesystem::path &file_path) {
            namespace fs = std::filesystem;

            if (!fs::exists(file_path)) {
                return {};
            }

            std::vector<HousingFurnitureCsv> furnitures;
            std::ifstream file(file_path);

            if (!file.is_open()) {
                return {};
            }

            std::string line;
            int line_count = 0;

            while (line_count < 3 && std::getline(file, line)) {
                line_count++;
            }

            while (std::getline(file, line)) {
                if (line.empty()) continue;

                auto fields = ParseCsvLine(line);
                if (fields.size() < 15) continue;

                try {
                    auto [key, custom_talk, name] = std::make_tuple(
                            std::stoi(fields[0]),
                            fields[7],
                            fields[8]
                    );

                    furnitures.push_back(HousingFurnitureCsv{
                            key, custom_talk, name
                    });
                } catch (const std::exception &e) {}
            }

            return furnitures;
        }

        static std::map<int, std::string> FurnitureKeyToNameMapping(const std::filesystem::path &path) {
            return Parse(path)
                   | std::views::filter([](const auto &furniture) { return !furniture.name.empty(); })
                   | std::views::transform([](const auto &furniture) {
                return std::pair{furniture.key, furniture.name};
            })
                   | std::ranges::to<std::map<int, std::string>>();
        }

        static std::vector<int> PreviewableFurnitureList(const std::filesystem::path &path) {
            return Parse(path)
                   | std::views::filter([](const auto &furniture) { return !furniture.name.empty(); })
                   | std::views::filter([](const auto &furniture) { return furniture.custom_talk.empty(); })
                   | std::views::transform([](const auto &furniture) { return furniture.key; })
                   | std::ranges::to<std::vector<int>>();
        }
    };
}
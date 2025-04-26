#pragma once

#include <any>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "file/struct/hbqj.h"
#include "file/struct/legacy.h"
#include "file/deserialization/deserializer.h"
#include "file/decryption/decryption_handler.h"
#include "log.h"

using json = nlohmann::json;

namespace hbqj {
    class __declspec(dllexport) FileReader {
    public:
        FileReader() {
            AddDecryptionHandler(std::make_unique<AesDecryptor>());

            RegisterDeserializer<FurnitureLayout>();
            RegisterDeserializer<HousingLayout>();
        }

        // delete copy constructor
        FileReader(const FileReader &) = delete;

        FileReader &operator=(const FileReader &) = delete;

        FileReader(FileReader &&) = default;

        FileReader &operator=(FileReader &&) = default;

        static std::vector<uint8_t> ReadBytesFromFile(const std::filesystem::path &path) {
            std::ifstream file(path, std::ios::binary);
            if (!file) {
                throw std::runtime_error("Cannot open file: " + path.string());
            }

            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> data(size);
            file.read(reinterpret_cast<char *>(data.data()), size);
            return data;
        }

        static bool IsJsonFile(const std::vector<uint8_t> &data) {
            try {
                std::string file_content(data.begin(), data.end());
                const auto &result = nlohmann::json::parse(file_content);
                return true;
            } catch (...) {
                return false;
            }
        }

        static bool IsBase64Encoded(const std::vector<uint8_t> &data) {
            auto isBase64Char = [](uint8_t c) {
                return (isalnum(c) || c == '+' || c == '/' || c == '=');
            };

            return std::all_of(data.begin(), data.end(), isBase64Char);
        }

        void AddDecryptionHandler(std::unique_ptr<IDecryptionHandler> handler) {
            decryption_handlers_.push_back(std::move(handler));
        }

        template<typename T>
        void RegisterDeserializer() {
            deserializers_[typeid(T).name()] = std::make_unique<IDeserializer<T>>();
        }

        template<typename T>
        std::optional<T> ReadFile(const std::filesystem::path &path) {
            std::vector<uint8_t> data = ReadBytesFromFile(path);
            log.info("Read file: {}, length: {}", path.string(), data.size());

            if (auto result = TryDeserialize<T>(data)) {
                return result;
            }

            log.info("Try to decrypt file..");

            for (const auto &handler: decryption_handlers_) {
                if (handler->isEncrypted(data)) {
                    auto decrypted_data = handler->decrypt(data);
                    log.info("Decrypted data length: {}", decrypted_data.size());
                    if (!decrypted_data.empty()) {
                        if (auto result = TryDeserialize<T>(decrypted_data)) {
                            return result;
                        }
                    }
                }
            }

            return std::nullopt;
        }

        struct DeserializationResult {
            std::string type_name;
            std::any data;
        };

        std::optional<DeserializationResult> ReadFileAutoDetect(const std::filesystem::path &path) {
            std::vector<uint8_t> data = ReadBytesFromFile(path);
            log.info("Read file: {}, length: {}", path.string(), data.size());

            if (auto result = TryDeserializeAll(data)) {
                return result;
            }

            log.info("Try to decrypt file..");

            for (const auto &handler: decryption_handlers_) {
                if (handler->isEncrypted(data)) {
                    auto decrypted_data = handler->decrypt(data);
                    log.info("Decrypted data length: {}", decrypted_data.size());
                    if (!decrypted_data.empty()) {
                        if (auto result = TryDeserializeAll(decrypted_data)) {
                            return result;
                        }
                    }
                }
            }

            return std::nullopt;
        }

        static HousingLayout ToHousingLayout(const DeserializationResult &result) {
            if (result.type_name == typeid(FurnitureLayout).name()) {
                return FurnitureLayoutToHousingLayout(std::any_cast<FurnitureLayout>(result.data));
            } else if (result.type_name == typeid(HousingLayout).name()) {
                return std::any_cast<HousingLayout>(result.data);
            }

            return {};
        }

    private:
        static HousingLayout FurnitureLayoutToHousingLayout(const FurnitureLayout &furniture_layout) {
            HousingLayout layout{
                    .metadata = {},
                    .items = furniture_layout.list
                             | std::views::transform(ExpandFurnitureItem)
                             | std::views::join
                             | std::ranges::to<std::vector>()
            };

            return layout;
        }

        static HousingItem ConvertFurnitureItemToHousingItem(const FurnitureItem &furniture, size_t index) {
            return HousingItem{
                    .type = static_cast<uint32_t>(furniture.categoryId),
                    .position = {
                            furniture.posX[index],
                            furniture.posY[index],
                            furniture.posZ[index]
                    },
                    .rotation = furniture.Rotation[index],
                    .color = furniture.Color ? (*furniture.Color)[index] : static_cast<uint8_t>(0)
            };
        }

        static std::vector<HousingItem> ExpandFurnitureItem(const FurnitureItem &furniture) {
            return std::views::iota(0, furniture.count)
                   | std::views::transform([&](size_t i) {
                return ConvertFurnitureItemToHousingItem(furniture, i);
            })
                   | std::ranges::to<std::vector>();
        }

        template<typename T>
        std::optional<T> TryDeserialize(const std::vector<uint8_t> &data) {
            if (!IsJsonFile(data)) {
                log.info("Not a valid json file.");
                return std::nullopt;
            }

            T result;
            auto deserializer = dynamic_cast<IDeserializer<T> *>(deserializers_[typeid(T).name()].get());
            if (deserializer && deserializer->tryDeserialize(data, result)) {
                return result;
            }

            return std::nullopt;
        }

        std::optional<DeserializationResult> TryDeserializeAll(const std::vector<uint8_t> &data) {
            if (!IsJsonFile(data)) {
                log.info("Not a valid json file.");
                return std::nullopt;
            }

            for (const auto &[type_name, deserializer]: deserializers_) {
                auto result = deserializer->tryDeserializeAny(data);
                if (result.has_value()) {
                    return DeserializationResult{type_name, result};
                }
            }

            return std::nullopt;
        }

        std::vector<std::unique_ptr<IDecryptionHandler>> decryption_handlers_;
        std::unordered_map<std::string, std::unique_ptr<IDeserializerBase>> deserializers_;
        Logger log = Logger::GetLogger("FileReader");
    };
}
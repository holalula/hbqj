#pragma once

#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include "file/deserialization/deserializer.h"
#include "file/decryption/decryption_handler.h"
#include "log.h"

using json = nlohmann::json;

namespace hbqj {
    class __declspec(dllexport) FileReader {
    public:
        FileReader() = default;

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

    private:
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

        std::vector<std::unique_ptr<IDecryptionHandler>> decryption_handlers_;
        std::unordered_map<std::string, std::unique_ptr<IDeserializerBase>> deserializers_;
        Logger log = Logger::GetLogger("FileReader");
    };
}
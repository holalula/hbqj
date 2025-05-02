#pragma once

#include <nlohmann/json.hpp>

#include "struct.h"
#include "log.h"

using json = nlohmann::json;

namespace hbqj {
    class __declspec(dllexport) File {
    public:
        static std::vector<uint8_t> ReadFile(const std::filesystem::path &path) {
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

        template<typename T>
        static bool SaveToFile(const std::filesystem::path &path, const T &value) {
            json j = value;

            try {
                std::ofstream json_file(path, std::ios::out);
                json_file << j;
                json_file.close();
            } catch (const std::filesystem::filesystem_error &e) {
                log.error("filesystem error: {}", e.what());
                return false;
            } catch (const nlohmann::json::exception &e) {
                log.error("json error: {}", e.what());
                return false;
            } catch (const std::ios_base::failure &e) {
                log.error("IO error: {}", e.what());
                return false;
            } catch (const std::exception &e) {
                log.error("save to file error: {}", e.what());
                return false;
            }


            log.info("Write file to path: {}", path.string());

            return true;
        }

        Position ReadPosition();

        void SavePosition();

        inline static Logger log = Logger::GetLogger("File");
    };
}
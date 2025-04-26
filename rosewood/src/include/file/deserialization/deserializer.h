#pragma once

#include <any>
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>

using json = nlohmann::json;

namespace hbqj {
    struct DeserializationResult {
        std::string type_name;
        std::any data;
    };

    class Deserializer {
    public:
        template<typename T>
        static std::optional<T> Deserialize(const std::vector<uint8_t> &data) {
            if (data.empty()) {
                return std::nullopt;
            }

            std::string json_str(data.begin(), data.end());

            T result;
            try {
                auto j = nlohmann::json::parse(json_str);

                from_json(j, result);

                return result;
            } catch (const std::exception &) {
                return std::nullopt;
            }
        }
    };
}
#pragma once

#include "file/deserialization/deserializer.h"
#include "file/struct/legacy.h"

namespace hbqj {
    template<>
    struct IDeserializer<FurnitureLayout> {
        bool tryDeserialize(const std::vector<uint8_t> &data, FurnitureLayout &pos) {
            try {
                std::string jsonStr(data.begin(), data.end());

                auto j = nlohmann::json::parse(jsonStr);

                return tryDeserialize(j, pos);
            } catch (const std::exception &) {
                return false;
            }
        }

        bool tryDeserialize(const nlohmann::json &j, FurnitureLayout &pos) {
            try {
                from_json(j, pos);
                return true;
            } catch (const std::exception &) {
                return false;
            }
        }
    };
}
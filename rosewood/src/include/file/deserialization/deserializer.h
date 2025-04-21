#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hbqj {
    template<typename T>
    struct IDeserializer {
        virtual bool tryDeserialize(const nlohmann::json &j, T &output) = 0;

        virtual ~IDeserializer() = default;
    };
}
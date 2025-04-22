#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>

using json = nlohmann::json;

namespace hbqj {
    class IDeserializerBase {
    public:
        virtual ~IDeserializerBase() = default;
    };

    template<typename T>
    struct IDeserializer : public IDeserializerBase {
        bool tryDeserialize(const nlohmann::json &j, T &output) { return false; };

        bool tryDeserialize(const std::vector<uint8_t> &data, T &output) { return false; };

        ~IDeserializer() = default;
    };
}
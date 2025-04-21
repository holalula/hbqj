#pragma once

#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "struct.h"

using json = nlohmann::json;

namespace hbqj {
    struct HousingLayout {
        std::map<std::string, std::string> metadata;
        std::vector<HousingItem> items;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(HousingLayout, metadata, items);
    };
}

template<>
struct std::formatter<hbqj::HousingLayout> : std::formatter<string> {
    auto format(const hbqj::HousingLayout &housing, format_context &ctx) const {
        nlohmann::json j = housing;
        return formatter<string>::format(j.dump(2), ctx);
    }
};
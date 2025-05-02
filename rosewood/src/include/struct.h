#pragma once

#include <optional>
#include <nlohmann/json.hpp>

#include "game_process.h"

using json = nlohmann::json;

namespace hbqj {
    struct Position {
        float x;
        float y;
        float z;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Position, x, y, z);
    };

    struct Quaternion {
        float x;
        float y;
        float z;
        float w;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Quaternion, x, y, z, w);
    };

    struct HousingItem {
        uint32_t type = 0; // key starts from 0x30000
        Position position = {0, 0, 0};
        float rotation = 0; // y-axis rotation in radians
        Byte color = 0;
        Address item_addr = 0;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(HousingItem, type, position, rotation, color);
    };

    struct HousingFile {

    };
}

template<>
struct std::formatter<hbqj::Position> : std::formatter<std::string> {
    auto format(const hbqj::Position &pos, format_context &ctx) const {
        return format_to(ctx.out(), "Position(x={:.2f}, y={:.2f}, z={:.2f})", pos.x, pos.y, pos.z);
    }
};

template<>
struct std::formatter<hbqj::Quaternion> : std::formatter<std::string> {
    auto format(const hbqj::Quaternion &rot, format_context &ctx) const {
        return format_to(ctx.out(), "Rotation(x={:.2f}, y={:.2f}, z={:.2f}, w={:.2f})", rot.x, rot.y, rot.z, rot.w);
    }
};

template<>
struct std::formatter<hbqj::HousingItem> : std::formatter<std::string> {
    auto format(const hbqj::HousingItem &item, format_context &ctx) const {
        return format_to(ctx.out(), "HousingItem(type={}, position={}, rotation={:.2f}, color={}, addr=0x{:x})",
                         item.type, item.position, item.rotation, static_cast<int>(item.color), item.item_addr);
    }
};

#pragma once

#include <optional>
#include <nlohmann/json.hpp>

#include "process.h"

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
        HousingItem(
                uint32_t t = 0,
                Position p = Position{0, 0, 0},
                float r = 0.0f,
                Byte c = 0,
                Address i = 0
        ) : type(t), position(p), rotation(r), color(c), item_addr(i) {}

        uint32_t type;
        Position position;
        float rotation;
        Byte color;
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

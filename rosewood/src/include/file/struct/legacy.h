#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <optional>
#include <format>

using json = nlohmann::json;

namespace hbqj {
    struct FurnitureItem {
        int categoryId = 0;
        int count = 0;
        std::vector<float> posX;
        std::vector<float> posY;
        std::vector<float> posZ;
        std::vector<float> Rotation;
        std::optional<std::vector<uint8_t >> Color;
    };

    static void from_json(const nlohmann::json &j, FurnitureItem &item) {
        j.at("categoryId").get_to(item.categoryId);
        j.at("count").get_to(item.count);
        j.at("posX").get_to(item.posX);
        j.at("posY").get_to(item.posY);
        j.at("posZ").get_to(item.posZ);
        j.at("Rotation").get_to(item.Rotation);

        if (j.contains("Color")) {
            item.Color = j.at("Color").get<std::vector<uint8_t>>();
        }
    }

    static void to_json(nlohmann::json &j, const FurnitureItem &item) {
        j = nlohmann::json{
                {"categoryId", item.categoryId},
                {"count",      item.count},
                {"posX",       item.posX},
                {"posY",       item.posY},
                {"posZ",       item.posZ},
                {"Rotation",   item.Rotation}
        };

        if (item.Color) {
            j["Color"] = *item.Color;
        }
    }

    struct FurnitureLayout {
        int size;
        std::vector<FurnitureItem> list;
    };

    static void from_json(const json &j, FurnitureLayout &layout) {
        j.at("size").get_to(layout.size);
        layout.list.clear();
        for (const auto &item: j.at("list")) {
            layout.list.push_back(item.get<FurnitureItem>());
        }
    }

    static void to_json(json &j, const FurnitureLayout &layout) {
        j = json{
                {"size", layout.size},
                {"list", layout.list}
        };
    }
}

template<>
struct std::formatter<hbqj::FurnitureItem> : std::formatter<string> {
    auto format(const hbqj::FurnitureItem &item, format_context &ctx) const {
        nlohmann::json j;
        hbqj::to_json(j, item);
        return formatter<string>::format(j.dump(2), ctx);
    }
};

template<>
struct std::formatter<hbqj::FurnitureLayout> : std::formatter<string> {
    auto format(const hbqj::FurnitureLayout &layout, format_context &ctx) const {
        nlohmann::json j;
        hbqj::to_json(j, layout);
        return formatter<string>::format(j.dump(2), ctx);
    }
};

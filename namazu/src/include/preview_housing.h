#pragma once

#include <cstdint>
#include <windef.h>

#include "logger.h"

namespace hbqj {
    typedef void (WINAPI* LoadHousingFunc)(int64_t housing, int64_t items);

    class PreviewHousing {
    public:
        static int64_t load_housing_func_offset;

        static LoadHousingFunc load_housing_func;

#pragma pack(1)
        struct LoadedHousingItem {
            uint16_t item_id;
            uint8_t  special_flag;   // 0x2
            uint8_t  _padding1;
            uint8_t  stain;          // 0x4
            uint8_t  _padding2[3];
            float    rotation;       // 0x8
            float    x;
            float    y;
            float    z;
        };

        struct LoadedHousingItemList {
            LoadedHousingItem list[100];
        };
#pragma pack()

        static void LoadHousingFuncHook(int64_t housing, int64_t items) {
            // log(std::format("Load: 0x{:x}, 0x{:x}", housing, items).c_str());
            // auto loaded_items = reinterpret_cast<LoadedHousingItemList*>(items + 12);
            // if (loaded_items) {
            //     for (int i = 0; i < 100; i++) {
            //         if (loaded_items->list[i].item_id != 0) {
            //             const auto& item = loaded_items->list[i];
            //             log(std::format("{}, {:.2f}, {:.2f}, {:.2f}",
            //                             item.item_id,
            //                             item.x,
            //                             item.y,
            //                             item.z).c_str());
            //         }
            //     }
            // }
            return load_housing_func(housing, items);
        }
    };

    LoadHousingFunc PreviewHousing::load_housing_func = nullptr;
    int64_t PreviewHousing::load_housing_func_offset = 0;
}
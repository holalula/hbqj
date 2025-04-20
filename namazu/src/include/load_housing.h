#pragma once

#include <cstdint>
#include <windef.h>

#include "logger.h"

namespace hbqj {
    using SelectItemFunc = void (WINAPI *)(int64_t housing, int64_t items);
    using PlaceItemFunc = void (WINAPI *)(int64_t housing, int64_t items);

    class LoadHousing {
    public:
        static int64_t select_item_func_offset;
        static int64_t place_item_func_offset;

        static SelectItemFunc select_item_func;
        static PlaceItemFunc place_item_func;

        static void SelectItemFuncHook(int64_t housing, int64_t items) {
            log(std::format("Select Item: 0x{:x}, 0x{:x}", housing, items).c_str());
            return select_item_func(housing, items);
        }

        static void PlaceItemFuncHook(int64_t housing, int64_t items) {
            log(std::format("Place Item: 0x{:x}, 0x{:x}", housing, items).c_str());
            return place_item_func(housing, items);
        }
    };

    int64_t LoadHousing::select_item_func_offset = 0;
    int64_t LoadHousing::place_item_func_offset = 0;

    SelectItemFunc LoadHousing::select_item_func = nullptr;
    PlaceItemFunc LoadHousing::place_item_func = nullptr;
}

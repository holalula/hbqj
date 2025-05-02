#pragma once

#include <cstdint>
#include <windef.h>

#include "logger.h"
#include "struct.h"

namespace hbqj {
    typedef void (WINAPI *LoadHousingFunc)(int64_t housing, int64_t items);

    class PreviewHousing {
    public:
        static int64_t load_housing_func_offset;

        static LoadHousingFunc load_housing_func;

#pragma pack(1)
        struct LoadedHousingItem {
            uint16_t item_id;
            uint8_t special_flag;   // 0x2
            uint8_t _padding1;
            uint8_t stain;          // 0x4
            uint8_t _padding2[3];
            float rotation;       // 0x8
            float x;
            float y;
            float z;
        };

        struct LoadedHousingItemList {
            LoadedHousingItem list[100];
        };
#pragma pack()

        static struct LoadHousingState {
            bool is_active = false;
            int total_items = 0;
            int loaded_items = 0;
            int expected_calls = 0;
            int current_call = 0;
            std::vector<HousingItem> items;

            void Reset() {
                is_active = false;
                total_items = 0;
                loaded_items = 0;
                expected_calls = 0;
                current_call = 0;
                items.clear();
            }

            void Start(const std::vector<HousingItem> &target_items, int count) {
                Reset();

                is_active = true;
                total_items = (std::min)(400, count);
                expected_calls = (total_items + 99) / 100;
                items = target_items;

                log(std::format("Start previewing housing layout, count = {}", total_items).c_str());
            }

            static bool IsTerminationSignal(const uint8_t *data) {
                for (int i = 0; i < 8; i++) {
                    if (data[i] != 0xFF) return false;
                }
                return true;
            }
        };

        static LoadHousingState load_state;

        static void PreviewHousingLayout(const std::vector<HousingItem> &items, int count) {
            load_state.Start(items, count);
        }

        static void LoadHousingFuncHook(int64_t housing, int64_t items) {
            log(std::format("Load: 0x{:x}, 0x{:x}", housing, items).c_str());

            if (items) {
                std::string bytes_str = "12 bytes: ";
                auto *bytes_ptr = reinterpret_cast<uint8_t *>(items);
                for (int i = 0; i < 12; i++) {
                    bytes_str += std::format("{:02X} ", bytes_ptr[i]);
                }
                log(bytes_str.c_str());

                if (LoadHousingState::IsTerminationSignal(bytes_ptr)) {
                    log("Stop loading items..");

                    load_state.Reset();

                    return load_housing_func(housing, items);
                }
            }

            if (load_state.is_active) {
                load_state.current_call++;

                log(std::format("Current load call: {} / {}.",
                                load_state.current_call, load_state.expected_calls).c_str());

                if (items) {
                    auto *loaded_items = reinterpret_cast<LoadedHousingItemList *>(items + 12);

                    if (loaded_items) {
                        memset(loaded_items, 0, sizeof(LoadedHousingItemList));

                        int start_idx = (load_state.current_call - 1) * 100;
                        int items_to_load = (std::min)(100, load_state.total_items - start_idx);

                        if (items_to_load > 0) {
                            log(std::format("Load items: {}-{}", start_idx, start_idx + items_to_load - 1).c_str());

                            for (int i = 0; i < items_to_load; i++) {
                                loaded_items->list[i].item_id = load_state.items[start_idx + i].type - 0x30000;
                                loaded_items->list[i].x = load_state.items[start_idx + i].position.x;
                                loaded_items->list[i].y = load_state.items[start_idx + i].position.y;
                                loaded_items->list[i].z = load_state.items[start_idx + i].position.z;
                                loaded_items->list[i].rotation = load_state.items[start_idx + i].rotation;
                                loaded_items->list[i].stain = load_state.items[start_idx + i].color;
                            }

                            load_state.loaded_items += items_to_load;
                        }
                    }
                }

                if (load_state.current_call >= load_state.expected_calls) {
                    log(std::format("Complete load custom housing items, total count: {}",
                                    load_state.loaded_items).c_str());

                    load_state.Reset();
                }

            }

            auto loaded_items = reinterpret_cast<LoadedHousingItemList *>(items + 12);
            if (loaded_items) {
                for (int i = 0; i < 100; i++) {
                    if (loaded_items->list[i].item_id != 0) {
                        const auto &item = loaded_items->list[i];
                        log(std::format("{}, {:.2f}, {:.2f}, {:.2f}",
                                        item.item_id,
                                        item.x,
                                        item.y,
                                        item.z).c_str());
                    }
                }
            }

            return load_housing_func(housing, items);
        }
    };

    LoadHousingFunc PreviewHousing::load_housing_func = nullptr;
    int64_t PreviewHousing::load_housing_func_offset = 0;
    PreviewHousing::LoadHousingState PreviewHousing::load_state;
}
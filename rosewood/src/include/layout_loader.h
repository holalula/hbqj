#pragma once

#include <vector>
#include <ranges>
#include <algorithm>

#include "struct.h"

namespace hbqj {
    struct LoadingPlanResult {
        std::vector<HousingItem> matched_items;
        std::vector<HousingItem> unmatched_current;
        std::vector<HousingItem> unmatched_target;
    };

    class __declspec(dllexport) LayoutLoader {
    private:
        static auto find_type_matches(const HousingItem &current_item,
                                      const std::vector<HousingItem> &target_items,
                                      const std::vector<bool> &is_matched) {
            return std::views::iota(size_t{0}, target_items.size())
                   | std::views::filter([&](size_t i) {
                return !is_matched[i] &&
                       target_items[i].type == current_item.type;
            })
                   | std::views::transform([&](size_t i) {
                return std::make_pair(i, std::ref(target_items[i]));
            });
        }

        static std::optional<size_t> find_best_match(
                const HousingItem &current_item,
                const std::vector<HousingItem> &target_items,
                const std::vector<bool> &is_matched) {

            auto matches = find_type_matches(current_item, target_items, is_matched);

            auto color_match = std::ranges::find_if(matches, [&](const auto &pair) {
                return pair.second.color == current_item.color;
            });

            if (color_match != std::ranges::end(matches)) {
                return (*color_match).first;
            }

            auto first_match = std::ranges::begin(matches);
            if (first_match != std::ranges::end(matches)) {
                return (*first_match).first;
            }

            return std::nullopt;
        }

    public:
        static LoadingPlanResult GetLoadingPlan(
                std::vector<HousingItem> &current_layout,
                std::vector<HousingItem> &target_layout) {

            std::vector<bool> is_matched(target_layout.size(), false);

            std::vector<HousingItem> loading_plan;
            loading_plan.reserve(current_layout.size());

            std::vector<HousingItem> unmatched_current;

            for (const auto &current_item: current_layout) {
                if (auto target_idx = find_best_match(current_item, target_layout, is_matched)) {
                    const auto &target_item = target_layout[*target_idx];
                    loading_plan.push_back({
                                                   .type = current_item.type,
                                                   .position = target_item.position,
                                                   .rotation = target_item.rotation,
                                                   .color = current_item.color,
                                                   .item_addr = current_item.item_addr
                                           });
                    is_matched[*target_idx] = true;
                } else {
                    unmatched_current.push_back(current_item);
                }
            }

            std::vector<HousingItem> unmatched_target;
            for (size_t i = 0; i < target_layout.size(); i++) {
                if (!is_matched[i]) {
                    unmatched_target.push_back(target_layout[i]);
                }
            }

            return LoadingPlanResult{
                    .matched_items = std::move(loading_plan),
                    .unmatched_current = std::move(unmatched_current),
                    .unmatched_target = std::move(unmatched_target),
            };
        }
    };
}
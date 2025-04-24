#include <gtest/gtest.h>

#include "layout_loader.h"

namespace hbqj {
    TEST(LayoutLoaderTest, ExactMatchTypeAndColor) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100},
                {.type = 2, .color = 2, .item_addr = 0x200}
        };

        std::vector<HousingItem> target = {
                {.type = 2, .position = {1, 1, 1}, .rotation = 90, .color = 2},
                {.type = 1, .position = {2, 2, 2}, .rotation = 180, .color = 1}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(result.matched_items.size(), 2);
        EXPECT_TRUE(result.unmatched_current.empty());
        EXPECT_TRUE(result.unmatched_target.empty());

        EXPECT_EQ(result.matched_items[0].type, 1);
        EXPECT_EQ(result.matched_items[0].color, 1);
        EXPECT_EQ(result.matched_items[0].item_addr, 0x100);
        EXPECT_EQ(result.matched_items[0].position.x, 2);
        EXPECT_EQ(result.matched_items[0].position.y, 2);
        EXPECT_EQ(result.matched_items[0].position.z, 2);
        EXPECT_EQ(result.matched_items[0].rotation, 180);

        EXPECT_EQ(result.matched_items[1].type, 2);
        EXPECT_EQ(result.matched_items[1].color, 2);
        EXPECT_EQ(result.matched_items[1].item_addr, 0x200);
        EXPECT_EQ(result.matched_items[1].position.x, 1);
        EXPECT_EQ(result.matched_items[1].position.y, 1);
        EXPECT_EQ(result.matched_items[1].position.z, 1);
        EXPECT_EQ(result.matched_items[1].rotation, 90);
    }

    TEST(LayoutLoaderTest, PreferColorMatch) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 2},
                {.type = 1, .position = {2, 2, 2}, .rotation = 180, .color = 1}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(result.matched_items.size(), 1);
        ASSERT_TRUE(result.unmatched_current.empty());
        ASSERT_EQ(result.unmatched_target.size(), 1);

        EXPECT_EQ(result.matched_items[0].position.x, 2);  // Should match with the color-matching item
        EXPECT_EQ(result.matched_items[0].rotation, 180);
    }

    TEST(LayoutLoaderTest, FallbackToFirstMatch) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 2},
                {.type = 1, .position = {2, 2, 2}, .rotation = 180, .color = 3}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(result.matched_items.size(), 1);
        ASSERT_TRUE(result.unmatched_current.empty());
        ASSERT_EQ(result.unmatched_target.size(), 1);

        EXPECT_EQ(result.matched_items[0].position.x, 1);  // Should match with the first available item
        EXPECT_EQ(result.matched_items[0].rotation, 90);
    }

    TEST(LayoutLoaderTest, NoMatchAvailable) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 2, .position = {1, 1, 1}, .rotation = 90, .color = 1},
                {.type = 3, .position = {2, 2, 2}, .rotation = 180, .color = 1}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        EXPECT_TRUE(result.matched_items.empty());
        EXPECT_EQ(result.unmatched_current.size(), 1);
        EXPECT_EQ(result.unmatched_target.size(), 2);
    }

    TEST(LayoutLoaderTest, PreventDoubleMatching) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100},
                {.type = 1, .color = 1, .item_addr = 0x200}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 1}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(result.matched_items.size(), 1);  // Should only match one item
        ASSERT_EQ(result.unmatched_current.size(), 1);
        ASSERT_TRUE(result.unmatched_target.empty());


        EXPECT_EQ(result.matched_items[0].item_addr, 0x100);  // Should match with the first current item
    }

    TEST(LayoutLoaderTest, PartialMatch) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100},
                {.type = 2, .color = 2, .item_addr = 0x200},
                {.type = 3, .color = 3, .item_addr = 0x300}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 1},
                {.type = 4, .position = {2, 2, 2}, .rotation = 180, .color = 4},
                {.type = 5, .position = {3, 3, 3}, .rotation = 270, .color = 5}
        };

        auto result = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(result.matched_items.size(), 1);
        EXPECT_EQ(result.matched_items[0].type, 1);
        EXPECT_EQ(result.matched_items[0].color, 1);
        EXPECT_EQ(result.matched_items[0].item_addr, 0x100);

        ASSERT_EQ(result.unmatched_current.size(), 2);
        EXPECT_EQ(result.unmatched_current[0].type, 2);
        EXPECT_EQ(result.unmatched_current[1].type, 3);

        ASSERT_EQ(result.unmatched_target.size(), 2);
        EXPECT_EQ(result.unmatched_target[0].type, 4);
        EXPECT_EQ(result.unmatched_target[1].type, 5);
    }
}
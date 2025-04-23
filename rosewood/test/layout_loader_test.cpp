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

        auto plan = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(plan.size(), 2);

        EXPECT_EQ(plan[0].type, 1);
        EXPECT_EQ(plan[0].color, 1);
        EXPECT_EQ(plan[0].item_addr, 0x100);
        EXPECT_EQ(plan[0].position.x, 2);
        EXPECT_EQ(plan[0].position.y, 2);
        EXPECT_EQ(plan[0].position.z, 2);
        EXPECT_EQ(plan[0].rotation, 180);

        EXPECT_EQ(plan[1].type, 2);
        EXPECT_EQ(plan[1].color, 2);
        EXPECT_EQ(plan[1].item_addr, 0x200);
        EXPECT_EQ(plan[1].position.x, 1);
        EXPECT_EQ(plan[1].position.y, 1);
        EXPECT_EQ(plan[1].position.z, 1);
        EXPECT_EQ(plan[1].rotation, 90);
    }

    TEST(LayoutLoaderTest, PreferColorMatch) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 2},
                {.type = 1, .position = {2, 2, 2}, .rotation = 180, .color = 1}
        };

        auto plan = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(plan.size(), 1);
        EXPECT_EQ(plan[0].position.x, 2);  // Should match with the color-matching item
        EXPECT_EQ(plan[0].rotation, 180);
    }

    TEST(LayoutLoaderTest, FallbackToFirstMatch) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 2},
                {.type = 1, .position = {2, 2, 2}, .rotation = 180, .color = 3}
        };

        auto plan = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(plan.size(), 1);
        EXPECT_EQ(plan[0].position.x, 1);  // Should match with the first available item
        EXPECT_EQ(plan[0].rotation, 90);
    }

    TEST(LayoutLoaderTest, NoMatchAvailable) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100}
        };

        std::vector<HousingItem> target = {
                {.type = 2, .position = {1, 1, 1}, .rotation = 90, .color = 1},
                {.type = 3, .position = {2, 2, 2}, .rotation = 180, .color = 1}
        };

        auto plan = LayoutLoader::GetLoadingPlan(current, target);

        EXPECT_TRUE(plan.empty());
    }

    TEST(LayoutLoaderTest, PreventDoubleMatching) {
        std::vector<HousingItem> current = {
                {.type = 1, .color = 1, .item_addr = 0x100},
                {.type = 1, .color = 1, .item_addr = 0x200}
        };

        std::vector<HousingItem> target = {
                {.type = 1, .position = {1, 1, 1}, .rotation = 90, .color = 1}
        };

        auto plan = LayoutLoader::GetLoadingPlan(current, target);

        ASSERT_EQ(plan.size(), 1);  // Should only match one item
        EXPECT_EQ(plan[0].item_addr, 0x100);  // Should match with the first current item
    }
}
#include <gtest/gtest.h>

#include "file.h"
#include "file/struct/hbqj.h"

namespace hbqj {
    TEST(FileTest, Example) {
        GTEST_SKIP();
        File f;
        f.ReadPosition();
        f.SavePosition();
    }

    TEST(FileTest, HbqjExample) {
        HousingLayout housing;

        housing.metadata = {
                {"key1", "value1"},
                {"key2", "value2"},
        };

        housing.items = {
                {
                        .type = 123,
                        .position = {1.1f, 2.2f, 3.3f},
                        .rotation = 1.2f,
                        .color = 2
                },
                {
                        .type = 456,
                        .position = {10.1f, 30.2f, 40.3f},
                        .rotation = 2.2f,
                        .color = 0
                }
        };

        File file;
        file.SaveToFile(std::filesystem::current_path() / "housing", housing);
    }
}

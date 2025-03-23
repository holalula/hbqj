#include <gtest/gtest.h>

TEST(ExampleTest, Test1) {
	GTEST_SKIP();
	EXPECT_TRUE(true);
	EXPECT_EQ(1, 1);
}
#include <gtest/gtest.h>

#include "helpers.h"

TEST(test_random, TestIntegerOne_One) {
    auto str = gen_random(6);
    ASSERT_EQ(6, str.size());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
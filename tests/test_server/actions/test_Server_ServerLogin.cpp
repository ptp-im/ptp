#include <gtest/gtest.h>

#include "test_init.h"
using namespace PTP::Common;

TEST(test_Server, ServerLoginAction) {
    test_int();
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

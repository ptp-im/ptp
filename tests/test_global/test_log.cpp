#include <gtest/gtest.h>
#include "ptp_global/Logger.h"

TEST(slog, debug) {
    DEBUG_I("DEBUG_I");
    DEBUG_D("DEBUG_D");
    DEBUG_E("DEBUG_E");
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "test_init.h"

TEST(cachepool, cachepool) {
    test_init();

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
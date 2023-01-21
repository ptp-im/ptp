#include <gtest/gtest.h>

#include "test_init.h"
#include "PTP.Auth.pb.h"

TEST(cachepool, cachepool) {
    test_init();
    CacheManager* pCacheManager = CacheManager::getInstance();
    ASSERT_TRUE(pCacheManager != NULL);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
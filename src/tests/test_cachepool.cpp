#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/ClientConn.h"
#include "ptp_business/CachePool.h"
#include "ptp_servers/boot.h"

TEST(cachepool, cachepool) {
    CacheManager::setConfigPath(CONFIG_PATH);
    CacheManager* pCacheManager = CacheManager::getInstance();
    ASSERT_TRUE(pCacheManager != NULL);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
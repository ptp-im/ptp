#include <gtest/gtest.h>
#include "ptp_global/Logger.h"
#include "tgnet/AccountManager.h"

TEST(AccountManager, upsertEntropy) {
    AccountManager::getInstance(101).upsertEntropy();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

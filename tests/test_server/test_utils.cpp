#include <gtest/gtest.h>

#include <array>
#include "ptp_global/Helpers.h"
#include "ptp_crypto/crypto_helpers.h"

TEST(test_random, gen_random_bytes) {
    unsigned char iv[16] = { 0 };
    gen_random_bytes(iv,16);
    ASSERT_TRUE(sizeof(iv) ==16);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
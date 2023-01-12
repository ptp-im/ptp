#include <gtest/gtest.h>
#include "ptp_toolbox/data/bytes_array.h"

TEST(toolbox, toolbox) {
    ptp_toolbox::data::bytes_array<32> seed;

    using bytes_data = ptp_toolbox::data::bytes_data;
    using bytes_64 = ptp_toolbox::data::bytes_array<64>;
    using bytes_32 = ptp_toolbox::data::bytes_array<32>;
    using bip32_key = ptp_toolbox::data::bytes_array<112>;

    ASSERT_EQ(1,1);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
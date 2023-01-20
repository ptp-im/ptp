#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/AuthCaptchaAction.h"
#include "PTP.Auth.pb.h"
#include "ptp_global/Helpers.h"

using namespace PTP::Common;

TEST(test_Auth, AuthCaptchaAction) {
    test_init();
    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

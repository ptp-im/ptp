#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_global/Timer.h"
//#include "ptp_servers/login/boot_login.h"

TEST(ptp_servers, boot_login) {
//    DEBUG_D("boot_route");
//    Timer t;
//    t.setTimeout([&]() {
//        DEBUG_D("timeout");
//        quit_login();
//        t.stop();
//    }, 3200);
//    int ret = boot_login(0, nullptr);
//    ASSERT_EQ(ret,0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

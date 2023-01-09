#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_global/Timer.h"
#include "ptp_global/NetLib.h"
#include "ptp_servers/boot.h"
#include "ptp_servers/msfs/boot_msfs.h"
#include "ptp_servers/route/boot_route.h"

TEST(ptp_servers, get_msfs_config) {
    DEBUG_D("get_msfs_config");
    char listen_ip[50];
    uint16_t listen_port;
    char base_dir[100];
    int filesPerDir;
    int nPostThreadCount;
    int nGetThreadCount;
    int ret = get_msfs_config(
            listen_ip,
            listen_port,
            base_dir,
            filesPerDir,
            nPostThreadCount,
            nGetThreadCount);

    ASSERT_EQ(ret,0);
}

TEST(ptp_servers, boot_msfs) {
    DEBUG_D("boot_msfs");
    Timer t;
    t.setTimeout([&]() {
        DEBUG_D("timeout");
        quit_msfs();
        t.stop();
    }, 3200);
    int ret = boot_msfs(0, nullptr);
    ASSERT_EQ(ret,0);
}


TEST(ptp_servers, boot_route) {
    DEBUG_D("boot_route");
    Timer t;
    t.setTimeout([&]() {
        DEBUG_D("timeout");
        quit_route();
        t.stop();
    }, 3200);
    int ret = boot_route(0, nullptr);
    ASSERT_EQ(ret,0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

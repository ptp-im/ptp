#include <gtest/gtest.h>
#include "slog_api.h"
#include "ptp_global/Logger.h"

TEST(slog, Test) {
    CSLog b("test2");
    b.Debug("www");
    CSLog a("test2");
    for (int i = 0; i < 10; i++) {
        a.Warn("aaa,%s", "bbb");
    }
}

TEST(slog, debug) {
    DEBUG_I("DEBUG_I");
    DEBUG_D("DEBUG_D");
    DEBUG_E("DEBUG_E");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

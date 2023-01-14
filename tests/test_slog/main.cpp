#include <gtest/gtest.h>
#include "slog_api.h"

TEST(slog, slog_set_append_enableStdout) {
    slog_set_append(true,true,false,"");
    CSLog b("test");
    b.Trace("stdout:true,debug:true,file:false");
    b.Debug("stdout:true,debug:true,file:false");
    b.Info("stdout:true,debug:true,file:false");
    slog_set_append(true,false,false,"");
    b.Trace("stdout:true,debug:false,file:false");
    b.Debug("stdout:true,debug:false,file:false");
    b.Info("stdout:true,debug:false,file:false");
    b.Warn("stdout:true,debug:false,file:false");
    b.Error("stdout:true,debug:false,file:false");
    ASSERT_TRUE(true);
}

TEST(slog, slog_set_append_disableStdout) {
    slog_set_append(false,true,true,"default.log");
    CSLog b("test");
    b.Debug("stdout:false,debug:true,file:true");
    ASSERT_TRUE(true);
}

TEST(slog, Test) {
    CSLog b("test2");
    b.Debug("www");
    CSLog a("test2");
    for (int i = 0; i < 10; i++) {
        a.Warn("aaa,%s", "bbb");
    }
    ASSERT_TRUE(true);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

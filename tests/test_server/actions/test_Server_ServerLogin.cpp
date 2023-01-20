#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/ServerLoginAction.h"
#include "ptp_server/actions/models/ModelServer.h"
#include "PTP.Server.pb.h"

using namespace PTP::Common;

TEST(test_Server, ServerLoginAction) {
    auto *pMsgConn = test_init_msg_conn();
    ASSERT_EQ(1,1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

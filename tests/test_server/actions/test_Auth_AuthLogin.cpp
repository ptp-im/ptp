#include <gtest/gtest.h>

#include "test_init.h"

using namespace PTP::Common;

TEST(test_Auth, AuthLoginAction) {
    test_init();
    CMsgSrvConn pMsgConn;
    auto handle = time(nullptr);
    pMsgConn.SetHandle(handle);
    login(&pMsgConn,10011);
    DEBUG_D("pMsgConn.GetHandle():%d",pMsgConn.GetHandle());
    DEBUG_D("pMsgConn.GetUserId():%d",pMsgConn.GetUserId());
    DEBUG_D("pMsgConn.GetAddress():%s",pMsgConn.GetAddress().c_str());
    ASSERT_TRUE(pMsgConn.GetHandle() > 0);
    ASSERT_TRUE(pMsgConn.GetUserId() > 0);
    ASSERT_TRUE(!pMsgConn.GetAddress().empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

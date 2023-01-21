#include <gtest/gtest.h>

#include "test_init.h"

using namespace PTP::Common;

TEST(test_Group, GroupCreateAction) {
    test_init();
    auto handle = (int)time(nullptr);
    CMsgSrvConn pMsgConn1;
    pMsgConn1.SetHandle(handle);
    login(&pMsgConn1,10011);
    uint32_t user_id_1 = pMsgConn1.GetUserId();

    CMsgSrvConn pMsgConn2;
    pMsgConn2.SetHandle(handle+1);
    login(&pMsgConn2,10012);
    uint32_t user_id_2 = pMsgConn2.GetUserId();

    CMsgSrvConn pMsgConn3;
    pMsgConn3.SetHandle(handle+2);
    login(&pMsgConn3,10013);
    uint32_t user_id_3 = pMsgConn3.GetUserId();

    auto groupType = PTP::Common::GROUP_TYPE_MULTI;
    createGroup(&pMsgConn1,groupType,user_id_1,user_id_2,10011);

    groupType = PTP::Common::GROUP_TYPE_PAIR;
    createGroup(&pMsgConn1,groupType,user_id_1,user_id_3,10011);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyModifyNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyModifyNotify msg_BuddyModifyNotify;
    msg_BuddyModifyNotify.set_buddy_modify_action(PTP::Common::BuddyModifyAction_sign_info);
    msg_BuddyModifyNotify.set_value("sign info");
    msg_BuddyModifyNotify.set_uid(pMsgConn->GetUserId());
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyModifyNotify;
    pdu_BuddyModifyNotify.SetPBMsg(&msg_BuddyModifyNotify,CID_BuddyModifyNotify,sep_no);
    CRequest request_BuddyModifyNotify;
    request_BuddyModifyNotify.SetHandle(pMsgConn->GetHandle());
    request_BuddyModifyNotify.SetRequestPdu(&pdu_BuddyModifyNotify);
    
    ACTION_BUDDY::BuddyModifyNotifyAction(&request_BuddyModifyNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

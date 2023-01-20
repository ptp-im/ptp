#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyStatNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyStatNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyStatNotify msg_BuddyStatNotify;
    msg_BuddyStatNotify.set_user_stat(USER_STAT_OFFLINE);
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyStatNotify;
    pdu_BuddyStatNotify.SetPBMsg(&msg_BuddyStatNotify,CID_BuddyStatNotify,sep_no);
    CRequest request_BuddyStatNotify;
    request_BuddyStatNotify.SetHandle(pMsgConn->GetHandle());
    request_BuddyStatNotify.SetRequestPdu(&pdu_BuddyStatNotify);
    
    ACTION_BUDDY::BuddyStatNotifyAction(&request_BuddyStatNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

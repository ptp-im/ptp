#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyUpdatePairNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyModifyUpdatePairNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyModifyUpdatePairNotify msg_BuddyModifyUpdatePairNotify;
    //msg_BuddyModifyUpdatePairNotify.set_pair_uid_list();
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyModifyUpdatePairNotify;
    pdu_BuddyModifyUpdatePairNotify.SetPBMsg(&msg_BuddyModifyUpdatePairNotify,CID_BuddyModifyUpdatePairNotify,sep_no);
    CRequest request_BuddyModifyUpdatePairNotify;
    request_BuddyModifyUpdatePairNotify.SetHandle(pMsgConn->GetHandle());
    request_BuddyModifyUpdatePairNotify.SetRequestPdu(&pdu_BuddyModifyUpdatePairNotify);
    
    ACTION_BUDDY::BuddyModifyUpdatePairNotifyAction(&request_BuddyModifyUpdatePairNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

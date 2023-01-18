#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyModifyNotifyAction) {
    test_int();
    PTP::Buddy::BuddyModifyNotify msg_BuddyModifyNotify;
    //msg_BuddyModifyNotify.set_buddy_modify_action();
    //msg_BuddyModifyNotify.set_uid();
    //msg_BuddyModifyNotify.set_value();
    uint16_t sep_no = 101;
    ImPdu pdu_BuddyModifyNotify;
    pdu_BuddyModifyNotify.SetPBMsg(&msg_BuddyModifyNotify,CID_BuddyModifyNotify,sep_no);
    CRequest request_BuddyModifyNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyModifyNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyModifyNotify.GetHandle(),pMsgConn);
    request_BuddyModifyNotify.SetRequestPdu(&pdu_BuddyModifyNotify);
    ACTION_BUDDY::BuddyModifyNotifyAction(&request_BuddyModifyNotify);
    if(request_BuddyModifyNotify.GetResponsePdu()){
        ASSERT_EQ(request_BuddyModifyNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

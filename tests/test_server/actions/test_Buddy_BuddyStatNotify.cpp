#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyStatNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyStatNotifyAction) {
    test_int();
    PTP::Buddy::BuddyStatNotify msg_BuddyStatNotify;
    //msg_BuddyStatNotify.set_user_stat();
    uint16_t sep_no = 101;
    ImPdu pdu_BuddyStatNotify;
    pdu_BuddyStatNotify.SetPBMsg(&msg_BuddyStatNotify,CID_BuddyStatNotify,sep_no);
    CRequest request_BuddyStatNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyStatNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyStatNotify.GetHandle(),pMsgConn);
    request_BuddyStatNotify.SetRequestPdu(&pdu_BuddyStatNotify);
    ACTION_BUDDY::BuddyStatNotifyAction(&request_BuddyStatNotify);
    if(request_BuddyStatNotify.GetResponsePdu()){
        ASSERT_EQ(request_BuddyStatNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

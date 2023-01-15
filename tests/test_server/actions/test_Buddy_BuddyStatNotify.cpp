#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyStatNotifyAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyStatNotifyAction) {
    test_int();
    PTP::Buddy::BuddyStatNotify msg_BuddyStatNotify;
    //msg_BuddyStatNotify.set_user_stat();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyStatNotify,CID_BuddyStatNotify,sep_no);
    CRequest request_BuddyStatNotify;
    CResponse response_BuddyStatNotify;
    request_BuddyStatNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyStatNotify.GetHandle(),new CMsgSrvConn());
    request_BuddyStatNotify.SetPdu(&pdu);
    ACTION_BUDDY::BuddyStatNotifyAction(&request_BuddyStatNotify,&response_BuddyStatNotify);
    if(response_BuddyStatNotify.GetPdu()){
        ASSERT_EQ(response_BuddyStatNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyNotifyAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyModifyNotifyAction) {
    test_int();
    PTP::Buddy::BuddyModifyNotify msg_BuddyModifyNotify;
    //msg_BuddyModifyNotify.set_buddy_modify_action();
    //msg_BuddyModifyNotify.set_uid();
    //msg_BuddyModifyNotify.set_value();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyModifyNotify,CID_BuddyModifyNotify,sep_no);
    CRequest request_BuddyModifyNotify;
    CResponse response_BuddyModifyNotify;
    request_BuddyModifyNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyModifyNotify.GetHandle(),new CMsgSrvConn());
    request_BuddyModifyNotify.SetPdu(&pdu);
    ACTION_BUDDY::BuddyModifyNotifyAction(&request_BuddyModifyNotify,&response_BuddyModifyNotify);
    if(response_BuddyModifyNotify.GetPdu()){
        ASSERT_EQ(response_BuddyModifyNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

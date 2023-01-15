#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyModifyAction) {
    test_int();
    PTP::Buddy::BuddyModifyReq msg_BuddyModifyReq;
    //msg_BuddyModifyReq.set_buddy_modify_action();
    //msg_BuddyModifyReq.set_value();
    //msg_BuddyModifyReq.set_attach_data();
    //msg_BuddyModifyReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyModifyReq,CID_BuddyModifyReq,sep_no);
    CRequest request_BuddyModifyReq;
    CResponse response_BuddyModifyReq;
    request_BuddyModifyReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyModifyReq.GetHandle(),new CMsgSrvConn());
    request_BuddyModifyReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyModifyReqAction(&request_BuddyModifyReq,&response_BuddyModifyReq);
    if(response_BuddyModifyReq.GetPdu()){
        ASSERT_EQ(response_BuddyModifyReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyModifyReq.GetPdu()->GetCommandId(),CID_BuddyModifyRes);
    PTP::Buddy::BuddyModifyRes msg_BuddyModifyRes;
    auto res = msg_BuddyModifyRes.ParseFromArray(response_BuddyModifyReq.GetPdu()->GetBodyData(), (int)response_BuddyModifyReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyModifyRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto buddy_modify_action = msg_BuddyModifyReq.buddy_modify_action();
    //DEBUG_I("buddy_modify_action: %p",buddy_modify_action);
    //auto value = msg_BuddyModifyReq.value();
    //DEBUG_I("value: %s",value.c_str());
    //auto notify_pairs = msg_BuddyModifyReq.notify_pairs();
    //DEBUG_I("notify_pairs: %p",notify_pairs);
    //auto attach_data = msg_BuddyModifyReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyModifyReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

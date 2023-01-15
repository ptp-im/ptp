#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetALLAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetALLAction) {
    test_int();
    PTP::Buddy::BuddyGetALLReq msg_BuddyGetALLReq;
    //msg_BuddyGetALLReq.set_buddy_updated_time();
    //msg_BuddyGetALLReq.set_attach_data();
    //msg_BuddyGetALLReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyGetALLReq,CID_BuddyGetALLReq,sep_no);
    CRequest request_BuddyGetALLReq;
    CResponse response_BuddyGetALLReq;
    request_BuddyGetALLReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyGetALLReq.GetHandle(),new CMsgSrvConn());
    request_BuddyGetALLReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyGetALLReqAction(&request_BuddyGetALLReq,&response_BuddyGetALLReq);
    if(response_BuddyGetALLReq.GetPdu()){
        ASSERT_EQ(response_BuddyGetALLReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyGetALLReq.GetPdu()->GetCommandId(),CID_BuddyGetALLRes);
    PTP::Buddy::BuddyGetALLRes msg_BuddyGetALLRes;
    auto res = msg_BuddyGetALLRes.ParseFromArray(response_BuddyGetALLReq.GetPdu()->GetBodyData(), (int)response_BuddyGetALLReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyGetALLRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto buddy_updated_time = msg_BuddyGetALLReq.buddy_updated_time();
    //DEBUG_I("buddy_updated_time: %d",buddy_updated_time);
    //auto buddy_list = msg_BuddyGetALLReq.buddy_list();
    //DEBUG_I("buddy_list: %p",buddy_list);
    //auto attach_data = msg_BuddyGetALLReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyGetALLReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyQueryListAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyQueryListAction) {
    test_int();
    PTP::Buddy::BuddyQueryListReq msg_BuddyQueryListReq;
    //msg_BuddyQueryListReq.set_params();
    //msg_BuddyQueryListReq.set_attach_data();
    //msg_BuddyQueryListReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyQueryListReq,CID_BuddyQueryListReq,sep_no);
    CRequest request_BuddyQueryListReq;
    CResponse response_BuddyQueryListReq;
    request_BuddyQueryListReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyQueryListReq.GetHandle(),new CMsgSrvConn());
    request_BuddyQueryListReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyQueryListReqAction(&request_BuddyQueryListReq,&response_BuddyQueryListReq);
    if(response_BuddyQueryListReq.GetPdu()){
        ASSERT_EQ(response_BuddyQueryListReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyQueryListReq.GetPdu()->GetCommandId(),CID_BuddyQueryListRes);
    PTP::Buddy::BuddyQueryListRes msg_BuddyQueryListRes;
    auto res = msg_BuddyQueryListRes.ParseFromArray(response_BuddyQueryListReq.GetPdu()->GetBodyData(), (int)response_BuddyQueryListReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyQueryListRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto buddy_list = msg_BuddyQueryListReq.buddy_list();
    //DEBUG_I("buddy_list: %p",buddy_list);
    //auto attach_data = msg_BuddyQueryListReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyQueryListReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

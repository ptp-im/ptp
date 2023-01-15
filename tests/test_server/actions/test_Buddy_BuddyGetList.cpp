#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetListAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetListAction) {
    test_int();
    PTP::Buddy::BuddyGetListReq msg_BuddyGetListReq;
    //msg_BuddyGetListReq.set_user_ids();
    //msg_BuddyGetListReq.set_attach_data();
    //msg_BuddyGetListReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyGetListReq,CID_BuddyGetListReq,sep_no);
    CRequest request_BuddyGetListReq;
    CResponse response_BuddyGetListReq;
    request_BuddyGetListReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyGetListReq.GetHandle(),new CMsgSrvConn());
    request_BuddyGetListReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyGetListReqAction(&request_BuddyGetListReq,&response_BuddyGetListReq);
    if(response_BuddyGetListReq.GetPdu()){
        ASSERT_EQ(response_BuddyGetListReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyGetListReq.GetPdu()->GetCommandId(),CID_BuddyGetListRes);
    PTP::Buddy::BuddyGetListRes msg_BuddyGetListRes;
    auto res = msg_BuddyGetListRes.ParseFromArray(response_BuddyGetListReq.GetPdu()->GetBodyData(), (int)response_BuddyGetListReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyGetListRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto buddy_list = msg_BuddyGetListReq.buddy_list();
    //DEBUG_I("buddy_list: %p",buddy_list);
    //auto attach_data = msg_BuddyGetListReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyGetListReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

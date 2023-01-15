#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetStatAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetStatAction) {
    test_int();
    PTP::Buddy::BuddyGetStatReq msg_BuddyGetStatReq;
    //msg_BuddyGetStatReq.set_user_ids();
    //msg_BuddyGetStatReq.set_attach_data();
    //msg_BuddyGetStatReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyGetStatReq,CID_BuddyGetStatReq,sep_no);
    CRequest request_BuddyGetStatReq;
    CResponse response_BuddyGetStatReq;
    request_BuddyGetStatReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyGetStatReq.GetHandle(),new CMsgSrvConn());
    request_BuddyGetStatReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyGetStatReqAction(&request_BuddyGetStatReq,&response_BuddyGetStatReq);
    if(response_BuddyGetStatReq.GetPdu()){
        ASSERT_EQ(response_BuddyGetStatReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyGetStatReq.GetPdu()->GetCommandId(),CID_BuddyGetStatRes);
    PTP::Buddy::BuddyGetStatRes msg_BuddyGetStatRes;
    auto res = msg_BuddyGetStatRes.ParseFromArray(response_BuddyGetStatReq.GetPdu()->GetBodyData(), (int)response_BuddyGetStatReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyGetStatRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto user_stat_list = msg_BuddyGetStatReq.user_stat_list();
    //DEBUG_I("user_stat_list: %p",user_stat_list);
    //auto attach_data = msg_BuddyGetStatReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyGetStatReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

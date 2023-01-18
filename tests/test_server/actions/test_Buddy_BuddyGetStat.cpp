#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetStatAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetStatAction) {
    test_int();
    PTP::Buddy::BuddyGetStatReq msg_BuddyGetStatReq;
    //msg_BuddyGetStatReq.set_user_ids();
    //msg_BuddyGetStatReq.set_attach_data();
    //msg_BuddyGetStatReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_BuddyGetStatReq;
    pdu_BuddyGetStatReq.SetPBMsg(&msg_BuddyGetStatReq,CID_BuddyGetStatReq,sep_no);
    CRequest request_BuddyGetStatReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyGetStatReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyGetStatReq.GetHandle(),pMsgConn);
    request_BuddyGetStatReq.SetRequestPdu(&pdu_BuddyGetStatReq);
    ACTION_BUDDY::BuddyGetStatReqAction(&request_BuddyGetStatReq);
    if(request_BuddyGetStatReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyGetStatReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyGetStatReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetStatReq);
        ASSERT_EQ(request_BuddyGetStatReq.IsNext(),true);

        CRequest request_next_BuddyGetStatReq;
        request_next_BuddyGetStatReq.SetIsBusinessConn(true);
        request_next_BuddyGetStatReq.SetRequestPdu(request_BuddyGetStatReq.GetResponsePdu());
        ACTION_BUDDY::BuddyGetStatReqAction(&request_next_BuddyGetStatReq);

        PTP::Buddy::BuddyGetStatRes msg_BuddyGetStatRes;
        auto res = msg_BuddyGetStatRes.ParseFromArray(request_next_BuddyGetStatReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyGetStatReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_BuddyGetStatReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetStatRes);
        auto error = msg_BuddyGetStatRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto user_stat_list = msg_BuddyGetStatRes.user_stat_list();
        //DEBUG_I("user_stat_list: %p",user_stat_list);
        //auto attach_data = msg_BuddyGetStatRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_BuddyGetStatRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_BuddyGetStatRes;
        request_BuddyGetStatRes.SetIsBusinessConn(false);
        request_BuddyGetStatRes.SetRequestPdu(request_next_BuddyGetStatReq.GetResponsePdu());
        ACTION_BUDDY::BuddyGetStatResAction(&request_BuddyGetStatRes);
        PTP::Buddy::BuddyGetStatRes msg_final_BuddyGetStatRes;
        res = msg_final_BuddyGetStatRes.ParseFromArray(request_BuddyGetStatRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetStatRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_BuddyGetStatRes.GetResponsePdu()->GetCommandId(),CID_BuddyGetStatRes);
        error = msg_final_BuddyGetStatRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

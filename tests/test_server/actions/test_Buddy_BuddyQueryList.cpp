#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyQueryListAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyQueryListAction) {
    test_int();
    PTP::Buddy::BuddyQueryListReq msg_BuddyQueryListReq;
    //msg_BuddyQueryListReq.set_params();
    //msg_BuddyQueryListReq.set_attach_data();
    //msg_BuddyQueryListReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_BuddyQueryListReq;
    pdu_BuddyQueryListReq.SetPBMsg(&msg_BuddyQueryListReq,CID_BuddyQueryListReq,sep_no);
    CRequest request_BuddyQueryListReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyQueryListReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyQueryListReq.GetHandle(),pMsgConn);
    request_BuddyQueryListReq.SetRequestPdu(&pdu_BuddyQueryListReq);
    ACTION_BUDDY::BuddyQueryListReqAction(&request_BuddyQueryListReq);
    if(request_BuddyQueryListReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyQueryListReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyQueryListReq.GetResponsePdu()->GetCommandId(),CID_BuddyQueryListReq);
        ASSERT_EQ(request_BuddyQueryListReq.IsNext(),true);

        CRequest request_next_BuddyQueryListReq;
        request_next_BuddyQueryListReq.SetIsBusinessConn(true);
        request_next_BuddyQueryListReq.SetRequestPdu(request_BuddyQueryListReq.GetResponsePdu());
        ACTION_BUDDY::BuddyQueryListReqAction(&request_next_BuddyQueryListReq);

        PTP::Buddy::BuddyQueryListRes msg_BuddyQueryListRes;
        auto res = msg_BuddyQueryListRes.ParseFromArray(request_next_BuddyQueryListReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyQueryListReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_BuddyQueryListReq.GetResponsePdu()->GetCommandId(),CID_BuddyQueryListRes);
        auto error = msg_BuddyQueryListRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto buddy_list = msg_BuddyQueryListRes.buddy_list();
        //DEBUG_I("buddy_list: %p",buddy_list);
        //auto attach_data = msg_BuddyQueryListRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_BuddyQueryListRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_BuddyQueryListRes;
        request_BuddyQueryListRes.SetIsBusinessConn(false);
        request_BuddyQueryListRes.SetRequestPdu(request_next_BuddyQueryListReq.GetResponsePdu());
        ACTION_BUDDY::BuddyQueryListResAction(&request_BuddyQueryListRes);
        PTP::Buddy::BuddyQueryListRes msg_final_BuddyQueryListRes;
        res = msg_final_BuddyQueryListRes.ParseFromArray(request_BuddyQueryListRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyQueryListRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_BuddyQueryListRes.GetResponsePdu()->GetCommandId(),CID_BuddyQueryListRes);
        error = msg_final_BuddyQueryListRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

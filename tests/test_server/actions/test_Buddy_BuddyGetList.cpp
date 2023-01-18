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
    ImPdu pdu_BuddyGetListReq;
    pdu_BuddyGetListReq.SetPBMsg(&msg_BuddyGetListReq,CID_BuddyGetListReq,sep_no);
    CRequest request_BuddyGetListReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyGetListReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyGetListReq.GetHandle(),pMsgConn);
    request_BuddyGetListReq.SetRequestPdu(&pdu_BuddyGetListReq);
    ACTION_BUDDY::BuddyGetListReqAction(&request_BuddyGetListReq);
    if(request_BuddyGetListReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyGetListReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyGetListReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetListReq);
        ASSERT_EQ(request_BuddyGetListReq.IsNext(),true);

        CRequest request_next_BuddyGetListReq;
        request_next_BuddyGetListReq.SetIsBusinessConn(true);
        request_next_BuddyGetListReq.SetRequestPdu(request_BuddyGetListReq.GetResponsePdu());
        ACTION_BUDDY::BuddyGetListReqAction(&request_next_BuddyGetListReq);

        PTP::Buddy::BuddyGetListRes msg_BuddyGetListRes;
        auto res = msg_BuddyGetListRes.ParseFromArray(request_next_BuddyGetListReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyGetListReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_BuddyGetListReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetListRes);
        auto error = msg_BuddyGetListRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto buddy_list = msg_BuddyGetListRes.buddy_list();
        //DEBUG_I("buddy_list: %p",buddy_list);
        //auto attach_data = msg_BuddyGetListRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_BuddyGetListRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_BuddyGetListRes;
        request_BuddyGetListRes.SetIsBusinessConn(false);
        request_BuddyGetListRes.SetRequestPdu(request_next_BuddyGetListReq.GetResponsePdu());
        ACTION_BUDDY::BuddyGetListResAction(&request_BuddyGetListRes);
        PTP::Buddy::BuddyGetListRes msg_final_BuddyGetListRes;
        res = msg_final_BuddyGetListRes.ParseFromArray(request_BuddyGetListRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetListRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_BuddyGetListRes.GetResponsePdu()->GetCommandId(),CID_BuddyGetListRes);
        error = msg_final_BuddyGetListRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

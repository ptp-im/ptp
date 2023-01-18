#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
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
    ImPdu pdu_BuddyModifyReq;
    pdu_BuddyModifyReq.SetPBMsg(&msg_BuddyModifyReq,CID_BuddyModifyReq,sep_no);
    CRequest request_BuddyModifyReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyModifyReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyModifyReq.GetHandle(),pMsgConn);
    request_BuddyModifyReq.SetRequestPdu(&pdu_BuddyModifyReq);
    ACTION_BUDDY::BuddyModifyReqAction(&request_BuddyModifyReq);
    if(request_BuddyModifyReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyModifyReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyModifyReq.GetResponsePdu()->GetCommandId(),CID_BuddyModifyReq);
        ASSERT_EQ(request_BuddyModifyReq.IsNext(),true);

        CRequest request_next_BuddyModifyReq;
        request_next_BuddyModifyReq.SetIsBusinessConn(true);
        request_next_BuddyModifyReq.SetRequestPdu(request_BuddyModifyReq.GetResponsePdu());
        ACTION_BUDDY::BuddyModifyReqAction(&request_next_BuddyModifyReq);

        PTP::Buddy::BuddyModifyRes msg_BuddyModifyRes;
        auto res = msg_BuddyModifyRes.ParseFromArray(request_next_BuddyModifyReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyModifyReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_BuddyModifyReq.GetResponsePdu()->GetCommandId(),CID_BuddyModifyRes);
        auto error = msg_BuddyModifyRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto buddy_modify_action = msg_BuddyModifyRes.buddy_modify_action();
        //DEBUG_I("buddy_modify_action: %p",buddy_modify_action);
        //auto value = msg_BuddyModifyRes.value();
        //DEBUG_I("value: %s",value.c_str());
        //auto notify_pairs = msg_BuddyModifyRes.notify_pairs();
        //DEBUG_I("notify_pairs: %p",notify_pairs);
        //auto attach_data = msg_BuddyModifyRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_BuddyModifyRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_BuddyModifyRes;
        request_BuddyModifyRes.SetIsBusinessConn(false);
        request_BuddyModifyRes.SetRequestPdu(request_next_BuddyModifyReq.GetResponsePdu());
        ACTION_BUDDY::BuddyModifyResAction(&request_BuddyModifyRes);
        PTP::Buddy::BuddyModifyRes msg_final_BuddyModifyRes;
        res = msg_final_BuddyModifyRes.ParseFromArray(request_BuddyModifyRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyModifyRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_BuddyModifyRes.GetResponsePdu()->GetCommandId(),CID_BuddyModifyRes);
        error = msg_final_BuddyModifyRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

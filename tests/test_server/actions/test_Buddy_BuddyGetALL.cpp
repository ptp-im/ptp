#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetALLAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetALLAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyGetALLReq msg_BuddyGetALLReq;
    msg_BuddyGetALLReq.set_buddy_updated_time(0);
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyGetALLReq;
    pdu_BuddyGetALLReq.SetPBMsg(&msg_BuddyGetALLReq,CID_BuddyGetALLReq,sep_no);
    CRequest request_BuddyGetALLReq;
    request_BuddyGetALLReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyGetALLReq.SetRequestPdu(&pdu_BuddyGetALLReq);
    
    ACTION_BUDDY::BuddyGetALLReqAction(&request_BuddyGetALLReq);
    
    if(request_BuddyGetALLReq.HasNext()){
        auto pdu_next_BuddyGetALLReq = request_BuddyGetALLReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyGetALLReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyGetALLReq->GetCommandId(),CID_BuddyGetALLReq);

        CRequest request_next_BuddyGetALLReq;
        request_next_BuddyGetALLReq.SetIsBusinessConn(true);
        request_next_BuddyGetALLReq.SetRequestPdu(pdu_next_BuddyGetALLReq);
        
        ACTION_BUDDY::BuddyGetALLReqAction(&request_next_BuddyGetALLReq);

        if(request_next_BuddyGetALLReq.HasNext()){}
        if(request_next_BuddyGetALLReq.GetResponsePdu()){
            PTP::Buddy::BuddyGetALLRes msg_BuddyGetALLRes;
            auto res = msg_BuddyGetALLRes.ParseFromArray(request_next_BuddyGetALLReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyGetALLReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyGetALLReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetALLRes);
            auto error = msg_BuddyGetALLRes.error();
            ASSERT_EQ(error,NO_ERROR);
            auto buddy_updated_time = msg_BuddyGetALLRes.buddy_updated_time();
            DEBUG_I("buddy_updated_time: %d",buddy_updated_time);
            auto auth_uid = msg_BuddyGetALLRes.auth_uid();
            DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_BuddyGetALLRes;
            request_BuddyGetALLRes.SetIsBusinessConn(false);
            request_BuddyGetALLRes.SetHandle(pMsgConn->GetHandle());
            request_BuddyGetALLRes.SetRequestPdu(request_next_BuddyGetALLReq.GetResponsePdu());
            
            ACTION_BUDDY::BuddyGetALLResAction(&request_BuddyGetALLRes);
            
            if(request_BuddyGetALLRes.HasNext()){}
            if(request_BuddyGetALLRes.GetResponsePdu()){
                PTP::Buddy::BuddyGetALLRes msg_final_BuddyGetALLRes;
                res = msg_final_BuddyGetALLRes.ParseFromArray(request_BuddyGetALLRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetALLRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_BuddyGetALLRes.GetResponsePdu()->GetCommandId(),CID_BuddyGetALLRes);
                error = msg_final_BuddyGetALLRes.error();
                ASSERT_EQ(error,NO_ERROR);
                for(const auto& buddy:msg_final_BuddyGetALLRes.buddy_list()){
                    DEBUG_I("==========>uid: %d",buddy.uid());
                    DEBUG_I("is_online: %d",buddy.is_online());
                    DEBUG_I("address: %s",buddy.address().c_str());
                    DEBUG_I("user_name: %s",buddy.user_name().c_str());
                    DEBUG_I("login_time: %d",buddy.login_time());
                }
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

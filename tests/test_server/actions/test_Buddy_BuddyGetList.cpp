#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetListAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetListAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyGetListReq msg_BuddyGetListReq;
    msg_BuddyGetListReq.add_user_ids(pMsgConn->GetUserId());
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyGetListReq;
    pdu_BuddyGetListReq.SetPBMsg(&msg_BuddyGetListReq,CID_BuddyGetListReq,sep_no);
    CRequest request_BuddyGetListReq;
    request_BuddyGetListReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyGetListReq.SetRequestPdu(&pdu_BuddyGetListReq);
    
    ACTION_BUDDY::BuddyGetListReqAction(&request_BuddyGetListReq);
    
    if(request_BuddyGetListReq.HasNext()){
        auto pdu_next_BuddyGetListReq = request_BuddyGetListReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyGetListReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyGetListReq->GetCommandId(),CID_BuddyGetListReq);

        CRequest request_next_BuddyGetListReq;
        request_next_BuddyGetListReq.SetIsBusinessConn(true);
        request_next_BuddyGetListReq.SetRequestPdu(pdu_next_BuddyGetListReq);
        
        ACTION_BUDDY::BuddyGetListReqAction(&request_next_BuddyGetListReq);

        if(request_next_BuddyGetListReq.HasNext()){}
        if(request_next_BuddyGetListReq.GetResponsePdu()){
            PTP::Buddy::BuddyGetListRes msg_BuddyGetListRes;
            auto res = msg_BuddyGetListRes.ParseFromArray(request_next_BuddyGetListReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyGetListReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyGetListReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetListRes);
            auto error = msg_BuddyGetListRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto buddy_list = msg_BuddyGetListRes.buddy_list();
            //DEBUG_I("buddy_list: %p",buddy_list);
            //auto auth_uid = msg_BuddyGetListRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_BuddyGetListRes;
            request_BuddyGetListRes.SetIsBusinessConn(false);
            request_BuddyGetListRes.SetHandle(pMsgConn->GetHandle());
            request_BuddyGetListRes.SetRequestPdu(request_next_BuddyGetListReq.GetResponsePdu());
            
            ACTION_BUDDY::BuddyGetListResAction(&request_BuddyGetListRes);
            
            if(request_BuddyGetListRes.HasNext()){}
            if(request_BuddyGetListRes.GetResponsePdu()){
                PTP::Buddy::BuddyGetListRes msg_final_BuddyGetListRes;
                res = msg_final_BuddyGetListRes.ParseFromArray(request_BuddyGetListRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetListRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_BuddyGetListRes.GetResponsePdu()->GetCommandId(),CID_BuddyGetListRes);
                error = msg_final_BuddyGetListRes.error();
                ASSERT_EQ(error,NO_ERROR);
                for(const auto& buddy:msg_final_BuddyGetListRes.buddy_list()){
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

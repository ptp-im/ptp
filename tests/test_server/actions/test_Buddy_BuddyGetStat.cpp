#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetStatAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetStatAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyGetStatReq msg_BuddyGetStatReq;
    msg_BuddyGetStatReq.add_user_ids(pMsgConn->GetUserId());
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyGetStatReq;
    pdu_BuddyGetStatReq.SetPBMsg(&msg_BuddyGetStatReq,CID_BuddyGetStatReq,sep_no);
    CRequest request_BuddyGetStatReq;
    request_BuddyGetStatReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyGetStatReq.SetRequestPdu(&pdu_BuddyGetStatReq);
    
    ACTION_BUDDY::BuddyGetStatReqAction(&request_BuddyGetStatReq);
    
    if(request_BuddyGetStatReq.HasNext()){
        auto pdu_next_BuddyGetStatReq = request_BuddyGetStatReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyGetStatReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyGetStatReq->GetCommandId(),CID_BuddyGetStatReq);

        CRequest request_next_BuddyGetStatReq;
        request_next_BuddyGetStatReq.SetIsBusinessConn(true);
        request_next_BuddyGetStatReq.SetRequestPdu(pdu_next_BuddyGetStatReq);
        
        ACTION_BUDDY::BuddyGetStatReqAction(&request_next_BuddyGetStatReq);

        if(request_next_BuddyGetStatReq.HasNext()){}
        if(request_next_BuddyGetStatReq.GetResponsePdu()){
            PTP::Buddy::BuddyGetStatRes msg_BuddyGetStatRes;
            auto res = msg_BuddyGetStatRes.ParseFromArray(request_next_BuddyGetStatReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyGetStatReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyGetStatReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetStatRes);
            auto error = msg_BuddyGetStatRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto user_stat_list = msg_BuddyGetStatRes.user_stat_list();
            //DEBUG_I("user_stat_list: %p",user_stat_list);
            //auto auth_uid = msg_BuddyGetStatRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_BuddyGetStatRes;
            request_BuddyGetStatRes.SetIsBusinessConn(false);
            request_BuddyGetStatRes.SetHandle(pMsgConn->GetHandle());
            request_BuddyGetStatRes.SetRequestPdu(request_next_BuddyGetStatReq.GetResponsePdu());
            
            ACTION_BUDDY::BuddyGetStatResAction(&request_BuddyGetStatRes);
            
            if(request_BuddyGetStatRes.HasNext()){}
            if(request_BuddyGetStatRes.GetResponsePdu()){
                PTP::Buddy::BuddyGetStatRes msg_final_BuddyGetStatRes;
                res = msg_final_BuddyGetStatRes.ParseFromArray(request_BuddyGetStatRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetStatRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_BuddyGetStatRes.GetResponsePdu()->GetCommandId(),CID_BuddyGetStatRes);
                error = msg_final_BuddyGetStatRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

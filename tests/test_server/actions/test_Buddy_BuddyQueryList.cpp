#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyQueryListAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyQueryListAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyQueryListReq msg_BuddyQueryListReq;

    auto *params = msg_BuddyQueryListReq.mutable_params();
    params->add_addresslist(pMsgConn->GetAddress());

    uint16_t sep_no = 1;
    ImPdu pdu_BuddyQueryListReq;
    pdu_BuddyQueryListReq.SetPBMsg(&msg_BuddyQueryListReq,CID_BuddyQueryListReq,sep_no);
    CRequest request_BuddyQueryListReq;
    request_BuddyQueryListReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyQueryListReq.SetRequestPdu(&pdu_BuddyQueryListReq);
    
    ACTION_BUDDY::BuddyQueryListReqAction(&request_BuddyQueryListReq);
    
    if(request_BuddyQueryListReq.HasNext()){
        auto pdu_next_BuddyQueryListReq = request_BuddyQueryListReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyQueryListReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyQueryListReq->GetCommandId(),CID_BuddyQueryListReq);

        CRequest request_next_BuddyQueryListReq;
        request_next_BuddyQueryListReq.SetIsBusinessConn(true);
        request_next_BuddyQueryListReq.SetRequestPdu(pdu_next_BuddyQueryListReq);
        
        ACTION_BUDDY::BuddyQueryListReqAction(&request_next_BuddyQueryListReq);

        if(request_next_BuddyQueryListReq.HasNext()){}
        if(request_next_BuddyQueryListReq.GetResponsePdu()){
            PTP::Buddy::BuddyQueryListRes msg_BuddyQueryListRes;
            auto res = msg_BuddyQueryListRes.ParseFromArray(request_next_BuddyQueryListReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyQueryListReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyQueryListReq.GetResponsePdu()->GetCommandId(),CID_BuddyQueryListRes);
            auto error = msg_BuddyQueryListRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto buddy_list = msg_BuddyQueryListRes.buddy_list();
            //DEBUG_I("buddy_list: %p",buddy_list);
            //auto auth_uid = msg_BuddyQueryListRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_BuddyQueryListRes;
            request_BuddyQueryListRes.SetIsBusinessConn(false);
            request_BuddyQueryListRes.SetHandle(pMsgConn->GetHandle());
            request_BuddyQueryListRes.SetRequestPdu(request_next_BuddyQueryListReq.GetResponsePdu());
            
            ACTION_BUDDY::BuddyQueryListResAction(&request_BuddyQueryListRes);
            
            if(request_BuddyQueryListRes.HasNext()){}
            if(request_BuddyQueryListRes.GetResponsePdu()){
                PTP::Buddy::BuddyQueryListRes msg_final_BuddyQueryListRes;
                res = msg_final_BuddyQueryListRes.ParseFromArray(request_BuddyQueryListRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyQueryListRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_BuddyQueryListRes.GetResponsePdu()->GetCommandId(),CID_BuddyQueryListRes);
                error = msg_final_BuddyQueryListRes.error();
                ASSERT_EQ(error,NO_ERROR);
                for(const auto& buddy:msg_final_BuddyQueryListRes.buddy_list()){
                    DEBUG_I("==========>uid: %d",buddy.uid());
                    DEBUG_I("is_online: %d",buddy.is_online());
                    DEBUG_I("address: %s",buddy.address().c_str());
                    DEBUG_I("user_name: %s",buddy.user_name().c_str());
                    DEBUG_I("pub_key: %s", bytes_to_hex_string((unsigned char *)buddy.pub_key().data(),33).c_str());
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

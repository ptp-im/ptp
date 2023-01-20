#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupUnreadMsgAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupUnreadMsgAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupUnreadMsgReq msg_GroupUnreadMsgReq;
    
    uint16_t sep_no = 1;
    ImPdu pdu_GroupUnreadMsgReq;
    pdu_GroupUnreadMsgReq.SetPBMsg(&msg_GroupUnreadMsgReq,CID_GroupUnreadMsgReq,sep_no);
    CRequest request_GroupUnreadMsgReq;
    request_GroupUnreadMsgReq.SetHandle(pMsgConn->GetHandle());
    request_GroupUnreadMsgReq.SetRequestPdu(&pdu_GroupUnreadMsgReq);
    
    ACTION_GROUP::GroupUnreadMsgReqAction(&request_GroupUnreadMsgReq);
    
    if(request_GroupUnreadMsgReq.HasNext()){
        auto pdu_next_GroupUnreadMsgReq = request_GroupUnreadMsgReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupUnreadMsgReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupUnreadMsgReq->GetCommandId(),CID_GroupUnreadMsgReq);

        CRequest request_next_GroupUnreadMsgReq;
        request_next_GroupUnreadMsgReq.SetIsBusinessConn(true);
        request_next_GroupUnreadMsgReq.SetRequestPdu(pdu_next_GroupUnreadMsgReq);
        
        ACTION_GROUP::GroupUnreadMsgReqAction(&request_next_GroupUnreadMsgReq);

        if(request_next_GroupUnreadMsgReq.HasNext()){}
        if(request_next_GroupUnreadMsgReq.GetResponsePdu()){
            PTP::Group::GroupUnreadMsgRes msg_GroupUnreadMsgRes;
            auto res = msg_GroupUnreadMsgRes.ParseFromArray(request_next_GroupUnreadMsgReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupUnreadMsgReq.GetResponsePdu()->GetBodyLength());
            //ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupUnreadMsgReq.GetResponsePdu()->GetCommandId(),CID_GroupUnreadMsgRes);
            auto error = msg_GroupUnreadMsgRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto unread_list = msg_GroupUnreadMsgRes.unread_list();
            //DEBUG_I("unread_list: %p",unread_list);
            //auto unread_cnt = msg_GroupUnreadMsgRes.unread_cnt();
            //DEBUG_I("unread_cnt: %d",unread_cnt);
            //auto auth_uid = msg_GroupUnreadMsgRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupUnreadMsgRes;
            request_GroupUnreadMsgRes.SetIsBusinessConn(false);
            request_GroupUnreadMsgRes.SetHandle(pMsgConn->GetHandle());
            request_GroupUnreadMsgRes.SetRequestPdu(request_next_GroupUnreadMsgReq.GetResponsePdu());
            
            ACTION_GROUP::GroupUnreadMsgResAction(&request_GroupUnreadMsgRes);
            
            if(request_GroupUnreadMsgRes.HasNext()){}
            if(request_GroupUnreadMsgRes.GetResponsePdu()){
                PTP::Group::GroupUnreadMsgRes msg_final_GroupUnreadMsgRes;
                res = msg_final_GroupUnreadMsgRes.ParseFromArray(request_GroupUnreadMsgRes.GetResponsePdu()->GetBodyData(), (int)request_GroupUnreadMsgRes.GetResponsePdu()->GetBodyLength());
//                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupUnreadMsgRes.GetResponsePdu()->GetCommandId(),CID_GroupUnreadMsgRes);
                error = msg_final_GroupUnreadMsgRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

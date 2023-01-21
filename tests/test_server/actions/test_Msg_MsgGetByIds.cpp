#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetByIdsAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetByIdsAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgGetByIdsReq msg_MsgGetByIdsReq;
    msg_MsgGetByIdsReq.set_group_id(1);
    msg_MsgGetByIdsReq.add_msg_ids(1);
    uint16_t sep_no = 1;
    ImPdu pdu_MsgGetByIdsReq;
    pdu_MsgGetByIdsReq.SetPBMsg(&msg_MsgGetByIdsReq,CID_MsgGetByIdsReq,sep_no);
    CRequest request_MsgGetByIdsReq;
    request_MsgGetByIdsReq.SetHandle(pMsgConn->GetHandle());
    request_MsgGetByIdsReq.SetRequestPdu(&pdu_MsgGetByIdsReq);
    
    ACTION_MSG::MsgGetByIdsReqAction(&request_MsgGetByIdsReq);
    
    if(request_MsgGetByIdsReq.HasNext()){
        auto pdu_next_MsgGetByIdsReq = request_MsgGetByIdsReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_MsgGetByIdsReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_MsgGetByIdsReq->GetCommandId(),CID_MsgGetByIdsReq);

        CRequest request_next_MsgGetByIdsReq;
        request_next_MsgGetByIdsReq.SetIsBusinessConn(true);
        request_next_MsgGetByIdsReq.SetRequestPdu(pdu_next_MsgGetByIdsReq);
        
        ACTION_MSG::MsgGetByIdsReqAction(&request_next_MsgGetByIdsReq);

        if(request_next_MsgGetByIdsReq.HasNext()){}
        if(request_next_MsgGetByIdsReq.GetResponsePdu()){
            PTP::Msg::MsgGetByIdsRes msg_MsgGetByIdsRes;
            auto res = msg_MsgGetByIdsRes.ParseFromArray(request_next_MsgGetByIdsReq.GetResponsePdu()->GetBodyData(), (int)request_next_MsgGetByIdsReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_MsgGetByIdsReq.GetResponsePdu()->GetCommandId(),CID_MsgGetByIdsRes);
            auto error = msg_MsgGetByIdsRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_id = msg_MsgGetByIdsRes.group_id();
            //DEBUG_I("group_id: %d",group_id);
            //auto msg_list = msg_MsgGetByIdsRes.msg_list();
            //DEBUG_I("msg_list: %p",msg_list);
            //auto auth_uid = msg_MsgGetByIdsRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_MsgGetByIdsRes;
            request_MsgGetByIdsRes.SetIsBusinessConn(false);
            request_MsgGetByIdsRes.SetHandle(pMsgConn->GetHandle());
            request_MsgGetByIdsRes.SetRequestPdu(request_next_MsgGetByIdsReq.GetResponsePdu());
            
            ACTION_MSG::MsgGetByIdsResAction(&request_MsgGetByIdsRes);
            
            if(request_MsgGetByIdsRes.HasNext()){}
            if(request_MsgGetByIdsRes.GetResponsePdu()){
                PTP::Msg::MsgGetByIdsRes msg_final_MsgGetByIdsRes;
                res = msg_final_MsgGetByIdsRes.ParseFromArray(request_MsgGetByIdsRes.GetResponsePdu()->GetBodyData(), (int)request_MsgGetByIdsRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_MsgGetByIdsRes.GetResponsePdu()->GetCommandId(),CID_MsgGetByIdsRes);
                error = msg_final_MsgGetByIdsRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

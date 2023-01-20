#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetMaxIdAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetMaxIdAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgGetMaxIdReq msg_MsgGetMaxIdReq;
    //msg_MsgGetMaxIdReq.set_group_id();
    uint16_t sep_no = 1;
    ImPdu pdu_MsgGetMaxIdReq;
    pdu_MsgGetMaxIdReq.SetPBMsg(&msg_MsgGetMaxIdReq,CID_MsgGetMaxIdReq,sep_no);
    CRequest request_MsgGetMaxIdReq;
    request_MsgGetMaxIdReq.SetHandle(pMsgConn->GetHandle());
    request_MsgGetMaxIdReq.SetRequestPdu(&pdu_MsgGetMaxIdReq);
    
    ACTION_MSG::MsgGetMaxIdReqAction(&request_MsgGetMaxIdReq);
    
    if(request_MsgGetMaxIdReq.HasNext()){
        auto pdu_next_MsgGetMaxIdReq = request_MsgGetMaxIdReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_MsgGetMaxIdReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_MsgGetMaxIdReq->GetCommandId(),CID_MsgGetMaxIdReq);

        CRequest request_next_MsgGetMaxIdReq;
        request_next_MsgGetMaxIdReq.SetIsBusinessConn(true);
        request_next_MsgGetMaxIdReq.SetRequestPdu(pdu_next_MsgGetMaxIdReq);
        
        ACTION_MSG::MsgGetMaxIdReqAction(&request_next_MsgGetMaxIdReq);

        if(request_next_MsgGetMaxIdReq.HasNext()){}
        if(request_next_MsgGetMaxIdReq.GetResponsePdu()){
            PTP::Msg::MsgGetMaxIdRes msg_MsgGetMaxIdRes;
            auto res = msg_MsgGetMaxIdRes.ParseFromArray(request_next_MsgGetMaxIdReq.GetResponsePdu()->GetBodyData(), (int)request_next_MsgGetMaxIdReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_MsgGetMaxIdReq.GetResponsePdu()->GetCommandId(),CID_MsgGetMaxIdRes);
            auto error = msg_MsgGetMaxIdRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_id = msg_MsgGetMaxIdRes.group_id();
            //DEBUG_I("group_id: %d",group_id);
            //auto msg_id = msg_MsgGetMaxIdRes.msg_id();
            //DEBUG_I("msg_id: %d",msg_id);
            //auto auth_uid = msg_MsgGetMaxIdRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_MsgGetMaxIdRes;
            request_MsgGetMaxIdRes.SetIsBusinessConn(false);
            request_MsgGetMaxIdRes.SetHandle(pMsgConn->GetHandle());
            request_MsgGetMaxIdRes.SetRequestPdu(request_next_MsgGetMaxIdReq.GetResponsePdu());
            
            ACTION_MSG::MsgGetMaxIdResAction(&request_MsgGetMaxIdRes);
            
            if(request_MsgGetMaxIdRes.HasNext()){}
            if(request_MsgGetMaxIdRes.GetResponsePdu()){
                PTP::Msg::MsgGetMaxIdRes msg_final_MsgGetMaxIdRes;
                res = msg_final_MsgGetMaxIdRes.ParseFromArray(request_MsgGetMaxIdRes.GetResponsePdu()->GetBodyData(), (int)request_MsgGetMaxIdRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_MsgGetMaxIdRes.GetResponsePdu()->GetCommandId(),CID_MsgGetMaxIdRes);
                error = msg_final_MsgGetMaxIdRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

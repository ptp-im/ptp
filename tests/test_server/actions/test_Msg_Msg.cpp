#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgAction) {
    test_int();
    PTP::Msg::MsgReq msg_MsgReq;
    //msg_MsgReq.set_group_adr();
    //msg_MsgReq.set_sent_at();
    //msg_MsgReq.set_msg_type();
    //msg_MsgReq.set_msg_data();
    //msg_MsgReq.set_attach_data();
    //msg_MsgReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgReq;
    pdu_MsgReq.SetPBMsg(&msg_MsgReq,CID_MsgReq,sep_no);
    CRequest request_MsgReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgReq.GetHandle(),pMsgConn);
    request_MsgReq.SetRequestPdu(&pdu_MsgReq);
    ACTION_MSG::MsgReqAction(&request_MsgReq);
    if(request_MsgReq.GetResponsePdu()){
        ASSERT_EQ(request_MsgReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_MsgReq.GetResponsePdu()->GetCommandId(),CID_MsgReq);
        ASSERT_EQ(request_MsgReq.IsNext(),true);

        CRequest request_next_MsgReq;
        request_next_MsgReq.SetIsBusinessConn(true);
        request_next_MsgReq.SetRequestPdu(request_MsgReq.GetResponsePdu());
        ACTION_MSG::MsgReqAction(&request_next_MsgReq);

        PTP::Msg::MsgRes msg_MsgRes;
        auto res = msg_MsgRes.ParseFromArray(request_next_MsgReq.GetResponsePdu()->GetBodyData(), (int)request_next_MsgReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_MsgReq.GetResponsePdu()->GetCommandId(),CID_MsgRes);
        auto error = msg_MsgRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto group_id = msg_MsgRes.group_id();
        //DEBUG_I("group_id: %d",group_id);
        //auto msg_id = msg_MsgRes.msg_id();
        //DEBUG_I("msg_id: %d",msg_id);
        //auto sent_at = msg_MsgRes.sent_at();
        //DEBUG_I("sent_at: %d",sent_at);
        //auto attach_data = msg_MsgRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_MsgRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_MsgRes;
        request_MsgRes.SetIsBusinessConn(false);
        request_MsgRes.SetRequestPdu(request_next_MsgReq.GetResponsePdu());
        ACTION_MSG::MsgResAction(&request_MsgRes);
        PTP::Msg::MsgRes msg_final_MsgRes;
        res = msg_final_MsgRes.ParseFromArray(request_MsgRes.GetResponsePdu()->GetBodyData(), (int)request_MsgRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_MsgRes.GetResponsePdu()->GetCommandId(),CID_MsgRes);
        error = msg_final_MsgRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

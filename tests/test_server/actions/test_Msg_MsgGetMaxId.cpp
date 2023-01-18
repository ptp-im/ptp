#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetMaxIdAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetMaxIdAction) {
    test_int();
    PTP::Msg::MsgGetMaxIdReq msg_MsgGetMaxIdReq;
    //msg_MsgGetMaxIdReq.set_group_id();
    //msg_MsgGetMaxIdReq.set_attach_data();
    //msg_MsgGetMaxIdReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgGetMaxIdReq;
    pdu_MsgGetMaxIdReq.SetPBMsg(&msg_MsgGetMaxIdReq,CID_MsgGetMaxIdReq,sep_no);
    CRequest request_MsgGetMaxIdReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgGetMaxIdReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgGetMaxIdReq.GetHandle(),pMsgConn);
    request_MsgGetMaxIdReq.SetRequestPdu(&pdu_MsgGetMaxIdReq);
    ACTION_MSG::MsgGetMaxIdReqAction(&request_MsgGetMaxIdReq);
    if(request_MsgGetMaxIdReq.GetResponsePdu()){
        ASSERT_EQ(request_MsgGetMaxIdReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_MsgGetMaxIdReq.GetResponsePdu()->GetCommandId(),CID_MsgGetMaxIdReq);
        ASSERT_EQ(request_MsgGetMaxIdReq.IsNext(),true);

        CRequest request_next_MsgGetMaxIdReq;
        request_next_MsgGetMaxIdReq.SetIsBusinessConn(true);
        request_next_MsgGetMaxIdReq.SetRequestPdu(request_MsgGetMaxIdReq.GetResponsePdu());
        ACTION_MSG::MsgGetMaxIdReqAction(&request_next_MsgGetMaxIdReq);

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
        //auto attach_data = msg_MsgGetMaxIdRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_MsgGetMaxIdRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_MsgGetMaxIdRes;
        request_MsgGetMaxIdRes.SetIsBusinessConn(false);
        request_MsgGetMaxIdRes.SetRequestPdu(request_next_MsgGetMaxIdReq.GetResponsePdu());
        ACTION_MSG::MsgGetMaxIdResAction(&request_MsgGetMaxIdRes);
        PTP::Msg::MsgGetMaxIdRes msg_final_MsgGetMaxIdRes;
        res = msg_final_MsgGetMaxIdRes.ParseFromArray(request_MsgGetMaxIdRes.GetResponsePdu()->GetBodyData(), (int)request_MsgGetMaxIdRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_MsgGetMaxIdRes.GetResponsePdu()->GetCommandId(),CID_MsgGetMaxIdRes);
        error = msg_final_MsgGetMaxIdRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

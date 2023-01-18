#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetByIdsAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetByIdsAction) {
    test_int();
    PTP::Msg::MsgGetByIdsReq msg_MsgGetByIdsReq;
    //msg_MsgGetByIdsReq.set_group_id();
    //msg_MsgGetByIdsReq.set_msg_ids();
    //msg_MsgGetByIdsReq.set_attach_data();
    //msg_MsgGetByIdsReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgGetByIdsReq;
    pdu_MsgGetByIdsReq.SetPBMsg(&msg_MsgGetByIdsReq,CID_MsgGetByIdsReq,sep_no);
    CRequest request_MsgGetByIdsReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgGetByIdsReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgGetByIdsReq.GetHandle(),pMsgConn);
    request_MsgGetByIdsReq.SetRequestPdu(&pdu_MsgGetByIdsReq);
    ACTION_MSG::MsgGetByIdsReqAction(&request_MsgGetByIdsReq);
    if(request_MsgGetByIdsReq.GetResponsePdu()){
        ASSERT_EQ(request_MsgGetByIdsReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_MsgGetByIdsReq.GetResponsePdu()->GetCommandId(),CID_MsgGetByIdsReq);
        ASSERT_EQ(request_MsgGetByIdsReq.IsNext(),true);

        CRequest request_next_MsgGetByIdsReq;
        request_next_MsgGetByIdsReq.SetIsBusinessConn(true);
        request_next_MsgGetByIdsReq.SetRequestPdu(request_MsgGetByIdsReq.GetResponsePdu());
        ACTION_MSG::MsgGetByIdsReqAction(&request_next_MsgGetByIdsReq);

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
        //auto attach_data = msg_MsgGetByIdsRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_MsgGetByIdsRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_MsgGetByIdsRes;
        request_MsgGetByIdsRes.SetIsBusinessConn(false);
        request_MsgGetByIdsRes.SetRequestPdu(request_next_MsgGetByIdsReq.GetResponsePdu());
        ACTION_MSG::MsgGetByIdsResAction(&request_MsgGetByIdsRes);
        PTP::Msg::MsgGetByIdsRes msg_final_MsgGetByIdsRes;
        res = msg_final_MsgGetByIdsRes.ParseFromArray(request_MsgGetByIdsRes.GetResponsePdu()->GetBodyData(), (int)request_MsgGetByIdsRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_MsgGetByIdsRes.GetResponsePdu()->GetCommandId(),CID_MsgGetByIdsRes);
        error = msg_final_MsgGetByIdsRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

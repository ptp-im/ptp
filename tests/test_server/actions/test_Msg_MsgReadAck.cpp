#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadAckAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgReadAckAction) {
    test_int();
    PTP::Msg::MsgReadAckReq msg_MsgReadAckReq;
    //msg_MsgReadAckReq.set_group_id();
    //msg_MsgReadAckReq.set_msg_id();
    //msg_MsgReadAckReq.set_attach_data();
    //msg_MsgReadAckReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgReadAckReq;
    pdu_MsgReadAckReq.SetPBMsg(&msg_MsgReadAckReq,CID_MsgReadAckReq,sep_no);
    CRequest request_MsgReadAckReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgReadAckReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgReadAckReq.GetHandle(),pMsgConn);
    request_MsgReadAckReq.SetRequestPdu(&pdu_MsgReadAckReq);
    ACTION_MSG::MsgReadAckReqAction(&request_MsgReadAckReq);
    if(request_MsgReadAckReq.GetResponsePdu()){
        ASSERT_EQ(request_MsgReadAckReq.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

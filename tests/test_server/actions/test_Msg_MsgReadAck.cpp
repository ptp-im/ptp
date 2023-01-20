#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadAckAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgReadAckAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgReadAckReq msg_MsgReadAckReq;
    //msg_MsgReadAckReq.set_group_id();
    //msg_MsgReadAckReq.set_msg_id();
    uint16_t sep_no = 1;
    ImPdu pdu_MsgReadAckReq;
    pdu_MsgReadAckReq.SetPBMsg(&msg_MsgReadAckReq,CID_MsgReadAckReq,sep_no);
    CRequest request_MsgReadAckReq;
    request_MsgReadAckReq.SetHandle(pMsgConn->GetHandle());
    request_MsgReadAckReq.SetRequestPdu(&pdu_MsgReadAckReq);
    
    ACTION_MSG::MsgReadAckReqAction(&request_MsgReadAckReq);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

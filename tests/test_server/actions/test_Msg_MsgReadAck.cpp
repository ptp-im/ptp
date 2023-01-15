#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadAckAction.h"
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
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgReadAckReq,CID_MsgReadAckReq,sep_no);
    CRequest request_MsgReadAckReq;
    CResponse response_MsgReadAckReq;
    request_MsgReadAckReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgReadAckReq.GetHandle(),new CMsgSrvConn());
    request_MsgReadAckReq.SetPdu(&pdu);
    ACTION_MSG::MsgReadAckReqAction(&request_MsgReadAckReq,&response_MsgReadAckReq);
    if(response_MsgReadAckReq.GetPdu()){
        ASSERT_EQ(response_MsgReadAckReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadAckAction.h"
#include "ptp_server/actions/MsgReadNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgReadAckAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgReadAckReq msg_MsgReadAckReq;
    msg_MsgReadAckReq.set_group_id(1);
    msg_MsgReadAckReq.set_msg_id(1);
    uint16_t sep_no = 1;
    ImPdu pdu_MsgReadAckReq;
    pdu_MsgReadAckReq.SetPBMsg(&msg_MsgReadAckReq,CID_MsgReadAckReq,sep_no);
    CRequest request_MsgReadAckReq;
    request_MsgReadAckReq.SetHandle(pMsgConn->GetHandle());
    request_MsgReadAckReq.SetRequestPdu(&pdu_MsgReadAckReq);
    
    ACTION_MSG::MsgReadAckReqAction(&request_MsgReadAckReq);

    if(request_MsgReadAckReq.HasNext()){
        CRequest request_next_MsgReadAckReq;
        request_next_MsgReadAckReq.SetIsBusinessConn(true);
        request_next_MsgReadAckReq.SetRequestPdu(request_MsgReadAckReq.GetNextResponsePdu());

        ACTION_MSG::MsgReadAckReqAction(&request_next_MsgReadAckReq);

        if(request_next_MsgReadAckReq.GetResponsePdu()){
            CRequest request_MsgReadNotify;
            request_MsgReadNotify.SetIsBusinessConn(false);
            request_MsgReadNotify.SetHandle(pMsgConn->GetHandle());
            request_MsgReadNotify.SetRequestPdu(request_next_MsgReadAckReq.GetResponsePdu());
            ACTION_MSG::MsgReadNotifyAction(&request_MsgReadNotify);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

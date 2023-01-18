#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgReadNotifyAction) {
    test_int();
    PTP::Msg::MsgReadNotify msg_MsgReadNotify;
    //msg_MsgReadNotify.set_group_id();
    //msg_MsgReadNotify.set_from_uid();
    //msg_MsgReadNotify.set_notify_users();
    //msg_MsgReadNotify.set_msg_id();
    //msg_MsgReadNotify.set_attach_data();
    //msg_MsgReadNotify.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgReadNotify;
    pdu_MsgReadNotify.SetPBMsg(&msg_MsgReadNotify,CID_MsgReadNotify,sep_no);
    CRequest request_MsgReadNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgReadNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgReadNotify.GetHandle(),pMsgConn);
    request_MsgReadNotify.SetRequestPdu(&pdu_MsgReadNotify);
    ACTION_MSG::MsgReadNotifyAction(&request_MsgReadNotify);
    if(request_MsgReadNotify.GetResponsePdu()){
        ASSERT_EQ(request_MsgReadNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgNotifyAction) {
    test_int();
    PTP::Msg::MsgNotify msg_MsgNotify;
    //msg_MsgNotify.set_msg_info();
    //msg_MsgNotify.set_notify_users();
    //msg_MsgNotify.set_attach_data();
    //msg_MsgNotify.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_MsgNotify;
    pdu_MsgNotify.SetPBMsg(&msg_MsgNotify,CID_MsgNotify,sep_no);
    CRequest request_MsgNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_MsgNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_MsgNotify.GetHandle(),pMsgConn);
    request_MsgNotify.SetRequestPdu(&pdu_MsgNotify);
    ACTION_MSG::MsgNotifyAction(&request_MsgNotify);
    if(request_MsgNotify.GetResponsePdu()){
        ASSERT_EQ(request_MsgNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

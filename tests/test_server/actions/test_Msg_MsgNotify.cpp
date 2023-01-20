#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgNotify msg_MsgNotify;
    //msg_MsgNotify.set_msg_info();
    //msg_MsgNotify.set_notify_users();
    uint16_t sep_no = 1;
    ImPdu pdu_MsgNotify;
    pdu_MsgNotify.SetPBMsg(&msg_MsgNotify,CID_MsgNotify,sep_no);
    CRequest request_MsgNotify;
    request_MsgNotify.SetHandle(pMsgConn->GetHandle());
    request_MsgNotify.SetRequestPdu(&pdu_MsgNotify);
    
    ACTION_MSG::MsgNotifyAction(&request_MsgNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgReadNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgReadNotify msg_MsgReadNotify;
    //msg_MsgReadNotify.set_group_id();
    //msg_MsgReadNotify.set_from_uid();
    //msg_MsgReadNotify.set_notify_users();
    //msg_MsgReadNotify.set_msg_id();
    uint16_t sep_no = 1;
    ImPdu pdu_MsgReadNotify;
    pdu_MsgReadNotify.SetPBMsg(&msg_MsgReadNotify,CID_MsgReadNotify,sep_no);
    CRequest request_MsgReadNotify;
    request_MsgReadNotify.SetHandle(pMsgConn->GetHandle());
    request_MsgReadNotify.SetRequestPdu(&pdu_MsgReadNotify);
    
    ACTION_MSG::MsgReadNotifyAction(&request_MsgReadNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

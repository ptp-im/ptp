#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgUnNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgUnNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Msg::MsgUnNotify msg_MsgUnNotify;
    //msg_MsgUnNotify.set_un_notify_users();
    //msg_MsgUnNotify.set_sent_at();
    //msg_MsgUnNotify.set_group_id();
    //msg_MsgUnNotify.set_from_uid();
    uint16_t sep_no = 1;
    ImPdu pdu_MsgUnNotify;
    pdu_MsgUnNotify.SetPBMsg(&msg_MsgUnNotify,CID_MsgUnNotify,sep_no);
    CRequest request_MsgUnNotify;
    request_MsgUnNotify.SetHandle(pMsgConn->GetHandle());
    request_MsgUnNotify.SetRequestPdu(&pdu_MsgUnNotify);
    
    ACTION_MSG::MsgUnNotifyAction(&request_MsgUnNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

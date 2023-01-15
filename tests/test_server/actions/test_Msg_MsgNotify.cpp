#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgNotifyAction.h"
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
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgNotify,CID_MsgNotify,sep_no);
    CRequest request_MsgNotify;
    CResponse response_MsgNotify;
    request_MsgNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgNotify.GetHandle(),new CMsgSrvConn());
    request_MsgNotify.SetPdu(&pdu);
    ACTION_MSG::MsgNotifyAction(&request_MsgNotify,&response_MsgNotify);
    if(response_MsgNotify.GetPdu()){
        ASSERT_EQ(response_MsgNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

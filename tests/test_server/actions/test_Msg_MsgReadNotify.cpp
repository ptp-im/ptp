#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgReadNotifyAction.h"
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
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgReadNotify,CID_MsgReadNotify,sep_no);
    CRequest request_MsgReadNotify;
    CResponse response_MsgReadNotify;
    request_MsgReadNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgReadNotify.GetHandle(),new CMsgSrvConn());
    request_MsgReadNotify.SetPdu(&pdu);
    ACTION_MSG::MsgReadNotifyAction(&request_MsgReadNotify,&response_MsgReadNotify);
    if(response_MsgReadNotify.GetPdu()){
        ASSERT_EQ(response_MsgReadNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

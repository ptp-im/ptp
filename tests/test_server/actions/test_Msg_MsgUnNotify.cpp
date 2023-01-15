#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgUnNotifyAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgUnNotifyAction) {
    test_int();
    PTP::Msg::MsgUnNotify msg_MsgUnNotify;
    //msg_MsgUnNotify.set_un_notify_users();
    //msg_MsgUnNotify.set_sent_at();
    //msg_MsgUnNotify.set_group_id();
    //msg_MsgUnNotify.set_from_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgUnNotify,CID_MsgUnNotify,sep_no);
    CRequest request_MsgUnNotify;
    CResponse response_MsgUnNotify;
    request_MsgUnNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgUnNotify.GetHandle(),new CMsgSrvConn());
    request_MsgUnNotify.SetPdu(&pdu);
    ACTION_MSG::MsgUnNotifyAction(&request_MsgUnNotify,&response_MsgUnNotify);
    if(response_MsgUnNotify.GetPdu()){
        ASSERT_EQ(response_MsgUnNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

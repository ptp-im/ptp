#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgAction) {
    test_int();
    PTP::Msg::MsgReq msg_MsgReq;
    //msg_MsgReq.set_group_adr();
    //msg_MsgReq.set_sent_at();
    //msg_MsgReq.set_msg_type();
    //msg_MsgReq.set_msg_data();
    //msg_MsgReq.set_attach_data();
    //msg_MsgReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgReq,CID_MsgReq,sep_no);
    CRequest request_MsgReq;
    CResponse response_MsgReq;
    request_MsgReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgReq.GetHandle(),new CMsgSrvConn());
    request_MsgReq.SetPdu(&pdu);
    ACTION_MSG::MsgReqAction(&request_MsgReq,&response_MsgReq);
    if(response_MsgReq.GetPdu()){
        ASSERT_EQ(response_MsgReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_MsgReq.GetPdu()->GetCommandId(),CID_MsgRes);
    PTP::Msg::MsgRes msg_MsgRes;
    auto res = msg_MsgRes.ParseFromArray(response_MsgReq.GetPdu()->GetBodyData(), (int)response_MsgReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_MsgRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_id = msg_MsgReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto msg_id = msg_MsgReq.msg_id();
    //DEBUG_I("msg_id: %d",msg_id);
    //auto sent_at = msg_MsgReq.sent_at();
    //DEBUG_I("sent_at: %d",sent_at);
    //auto attach_data = msg_MsgReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_MsgReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

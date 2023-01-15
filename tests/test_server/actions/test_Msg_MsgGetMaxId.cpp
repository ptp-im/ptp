#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetMaxIdAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetMaxIdAction) {
    test_int();
    PTP::Msg::MsgGetMaxIdReq msg_MsgGetMaxIdReq;
    //msg_MsgGetMaxIdReq.set_group_id();
    //msg_MsgGetMaxIdReq.set_attach_data();
    //msg_MsgGetMaxIdReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgGetMaxIdReq,CID_MsgGetMaxIdReq,sep_no);
    CRequest request_MsgGetMaxIdReq;
    CResponse response_MsgGetMaxIdReq;
    request_MsgGetMaxIdReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgGetMaxIdReq.GetHandle(),new CMsgSrvConn());
    request_MsgGetMaxIdReq.SetPdu(&pdu);
    ACTION_MSG::MsgGetMaxIdReqAction(&request_MsgGetMaxIdReq,&response_MsgGetMaxIdReq);
    if(response_MsgGetMaxIdReq.GetPdu()){
        ASSERT_EQ(response_MsgGetMaxIdReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_MsgGetMaxIdReq.GetPdu()->GetCommandId(),CID_MsgGetMaxIdRes);
    PTP::Msg::MsgGetMaxIdRes msg_MsgGetMaxIdRes;
    auto res = msg_MsgGetMaxIdRes.ParseFromArray(response_MsgGetMaxIdReq.GetPdu()->GetBodyData(), (int)response_MsgGetMaxIdReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_MsgGetMaxIdRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_id = msg_MsgGetMaxIdReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto msg_id = msg_MsgGetMaxIdReq.msg_id();
    //DEBUG_I("msg_id: %d",msg_id);
    //auto attach_data = msg_MsgGetMaxIdReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_MsgGetMaxIdReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

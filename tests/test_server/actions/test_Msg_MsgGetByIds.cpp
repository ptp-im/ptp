#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgGetByIdsAction.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgGetByIdsAction) {
    test_int();
    PTP::Msg::MsgGetByIdsReq msg_MsgGetByIdsReq;
    //msg_MsgGetByIdsReq.set_group_id();
    //msg_MsgGetByIdsReq.set_msg_ids();
    //msg_MsgGetByIdsReq.set_attach_data();
    //msg_MsgGetByIdsReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_MsgGetByIdsReq,CID_MsgGetByIdsReq,sep_no);
    CRequest request_MsgGetByIdsReq;
    CResponse response_MsgGetByIdsReq;
    request_MsgGetByIdsReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_MsgGetByIdsReq.GetHandle(),new CMsgSrvConn());
    request_MsgGetByIdsReq.SetPdu(&pdu);
    ACTION_MSG::MsgGetByIdsReqAction(&request_MsgGetByIdsReq,&response_MsgGetByIdsReq);
    if(response_MsgGetByIdsReq.GetPdu()){
        ASSERT_EQ(response_MsgGetByIdsReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_MsgGetByIdsReq.GetPdu()->GetCommandId(),CID_MsgGetByIdsRes);
    PTP::Msg::MsgGetByIdsRes msg_MsgGetByIdsRes;
    auto res = msg_MsgGetByIdsRes.ParseFromArray(response_MsgGetByIdsReq.GetPdu()->GetBodyData(), (int)response_MsgGetByIdsReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_MsgGetByIdsRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_id = msg_MsgGetByIdsReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto msg_list = msg_MsgGetByIdsReq.msg_list();
    //DEBUG_I("msg_list: %p",msg_list);
    //auto attach_data = msg_MsgGetByIdsReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_MsgGetByIdsReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

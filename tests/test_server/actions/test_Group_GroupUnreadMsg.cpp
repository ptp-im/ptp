#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupUnreadMsgAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupUnreadMsgAction) {
    test_int();
    PTP::Group::GroupUnreadMsgReq msg_GroupUnreadMsgReq;
    //msg_GroupUnreadMsgReq.set_attach_data();
    //msg_GroupUnreadMsgReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupUnreadMsgReq,CID_GroupUnreadMsgReq,sep_no);
    CRequest request_GroupUnreadMsgReq;
    CResponse response_GroupUnreadMsgReq;
    request_GroupUnreadMsgReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupUnreadMsgReq.GetHandle(),new CMsgSrvConn());
    request_GroupUnreadMsgReq.SetPdu(&pdu);
    ACTION_GROUP::GroupUnreadMsgReqAction(&request_GroupUnreadMsgReq,&response_GroupUnreadMsgReq);
    if(response_GroupUnreadMsgReq.GetPdu()){
        ASSERT_EQ(response_GroupUnreadMsgReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupUnreadMsgReq.GetPdu()->GetCommandId(),CID_GroupUnreadMsgRes);
    PTP::Group::GroupUnreadMsgRes msg_GroupUnreadMsgRes;
    auto res = msg_GroupUnreadMsgRes.ParseFromArray(response_GroupUnreadMsgReq.GetPdu()->GetBodyData(), (int)response_GroupUnreadMsgReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupUnreadMsgRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto unread_list = msg_GroupUnreadMsgReq.unread_list();
    //DEBUG_I("unread_list: %p",unread_list);
    //auto unread_cnt = msg_GroupUnreadMsgReq.unread_cnt();
    //DEBUG_I("unread_cnt: %d",unread_cnt);
    //auto attach_data = msg_GroupUnreadMsgReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupUnreadMsgReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

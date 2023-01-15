#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupCreateAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupCreateAction) {
    test_int();
    PTP::Group::GroupCreateReq msg_GroupCreateReq;
    //msg_GroupCreateReq.set_group_idx();
    //msg_GroupCreateReq.set_sign();
    //msg_GroupCreateReq.set_captcha();
    //msg_GroupCreateReq.set_group_type();
    //msg_GroupCreateReq.set_name();
    //msg_GroupCreateReq.set_avatar();
    //msg_GroupCreateReq.set_members();
    //msg_GroupCreateReq.set_about();
    //msg_GroupCreateReq.set_attach_data();
    //msg_GroupCreateReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupCreateReq,CID_GroupCreateReq,sep_no);
    CRequest request_GroupCreateReq;
    CResponse response_GroupCreateReq;
    request_GroupCreateReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupCreateReq.GetHandle(),new CMsgSrvConn());
    request_GroupCreateReq.SetPdu(&pdu);
    ACTION_GROUP::GroupCreateReqAction(&request_GroupCreateReq,&response_GroupCreateReq);
    if(response_GroupCreateReq.GetPdu()){
        ASSERT_EQ(response_GroupCreateReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupCreateReq.GetPdu()->GetCommandId(),CID_GroupCreateRes);
    PTP::Group::GroupCreateRes msg_GroupCreateRes;
    auto res = msg_GroupCreateRes.ParseFromArray(response_GroupCreateReq.GetPdu()->GetBodyData(), (int)response_GroupCreateReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupCreateRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group = msg_GroupCreateReq.group();
    //DEBUG_I("group: %p",group);
    //auto group_members = msg_GroupCreateReq.group_members();
    //DEBUG_I("group_members: %p",group_members);
    //auto attach_data = msg_GroupCreateReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupCreateReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

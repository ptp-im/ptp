#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupModifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupModifyAction) {
    test_int();
    PTP::Group::GroupModifyReq msg_GroupModifyReq;
    //msg_GroupModifyReq.set_group_modify_action();
    //msg_GroupModifyReq.set_group_id();
    //msg_GroupModifyReq.set_value();
    //msg_GroupModifyReq.set_attach_data();
    //msg_GroupModifyReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupModifyReq,CID_GroupModifyReq,sep_no);
    CRequest request_GroupModifyReq;
    CResponse response_GroupModifyReq;
    request_GroupModifyReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupModifyReq.GetHandle(),new CMsgSrvConn());
    request_GroupModifyReq.SetPdu(&pdu);
    ACTION_GROUP::GroupModifyReqAction(&request_GroupModifyReq,&response_GroupModifyReq);
    if(response_GroupModifyReq.GetPdu()){
        ASSERT_EQ(response_GroupModifyReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupModifyReq.GetPdu()->GetCommandId(),CID_GroupModifyRes);
    PTP::Group::GroupModifyRes msg_GroupModifyRes;
    auto res = msg_GroupModifyRes.ParseFromArray(response_GroupModifyReq.GetPdu()->GetBodyData(), (int)response_GroupModifyReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupModifyRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_modify_action = msg_GroupModifyReq.group_modify_action();
    //DEBUG_I("group_modify_action: %p",group_modify_action);
    //auto value = msg_GroupModifyReq.value();
    //DEBUG_I("value: %s",value.c_str());
    //auto group_id = msg_GroupModifyReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto notify_members = msg_GroupModifyReq.notify_members();
    //DEBUG_I("notify_members: %p",notify_members);
    //auto attach_data = msg_GroupModifyReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupModifyReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

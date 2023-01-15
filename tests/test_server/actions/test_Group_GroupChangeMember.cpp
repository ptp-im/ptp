#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberAction) {
    test_int();
    PTP::Group::GroupChangeMemberReq msg_GroupChangeMemberReq;
    //msg_GroupChangeMemberReq.set_group_member_modify_action();
    //msg_GroupChangeMemberReq.set_group_id();
    //msg_GroupChangeMemberReq.set_members();
    //msg_GroupChangeMemberReq.set_attach_data();
    //msg_GroupChangeMemberReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupChangeMemberReq,CID_GroupChangeMemberReq,sep_no);
    CRequest request_GroupChangeMemberReq;
    CResponse response_GroupChangeMemberReq;
    request_GroupChangeMemberReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupChangeMemberReq.GetHandle(),new CMsgSrvConn());
    request_GroupChangeMemberReq.SetPdu(&pdu);
    ACTION_GROUP::GroupChangeMemberReqAction(&request_GroupChangeMemberReq,&response_GroupChangeMemberReq);
    if(response_GroupChangeMemberReq.GetPdu()){
        ASSERT_EQ(response_GroupChangeMemberReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupChangeMemberReq.GetPdu()->GetCommandId(),CID_GroupChangeMemberRes);
    PTP::Group::GroupChangeMemberRes msg_GroupChangeMemberRes;
    auto res = msg_GroupChangeMemberRes.ParseFromArray(response_GroupChangeMemberReq.GetPdu()->GetBodyData(), (int)response_GroupChangeMemberReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupChangeMemberRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_member_modify_action = msg_GroupChangeMemberReq.group_member_modify_action();
    //DEBUG_I("group_member_modify_action: %p",group_member_modify_action);
    //auto group_id = msg_GroupChangeMemberReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto members_chang = msg_GroupChangeMemberReq.members_chang();
    //DEBUG_I("members_chang: %p",members_chang);
    //auto attach_data = msg_GroupChangeMemberReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupChangeMemberReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

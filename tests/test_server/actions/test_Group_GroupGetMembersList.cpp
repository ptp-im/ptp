#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupGetMembersListAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupGetMembersListAction) {
    test_int();
    PTP::Group::GroupGetMembersListReq msg_GroupGetMembersListReq;
    //msg_GroupGetMembersListReq.set_group_members_updated_time();
    //msg_GroupGetMembersListReq.set_group_id();
    //msg_GroupGetMembersListReq.set_attach_data();
    //msg_GroupGetMembersListReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupGetMembersListReq,CID_GroupGetMembersListReq,sep_no);
    CRequest request_GroupGetMembersListReq;
    CResponse response_GroupGetMembersListReq;
    request_GroupGetMembersListReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupGetMembersListReq.GetHandle(),new CMsgSrvConn());
    request_GroupGetMembersListReq.SetPdu(&pdu);
    ACTION_GROUP::GroupGetMembersListReqAction(&request_GroupGetMembersListReq,&response_GroupGetMembersListReq);
    if(response_GroupGetMembersListReq.GetPdu()){
        ASSERT_EQ(response_GroupGetMembersListReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupGetMembersListReq.GetPdu()->GetCommandId(),CID_GroupGetMembersListRes);
    PTP::Group::GroupGetMembersListRes msg_GroupGetMembersListRes;
    auto res = msg_GroupGetMembersListRes.ParseFromArray(response_GroupGetMembersListReq.GetPdu()->GetBodyData(), (int)response_GroupGetMembersListReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupGetMembersListRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_members_updated_time = msg_GroupGetMembersListReq.group_members_updated_time();
    //DEBUG_I("group_members_updated_time: %d",group_members_updated_time);
    //auto group_id = msg_GroupGetMembersListReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto members = msg_GroupGetMembersListReq.members();
    //DEBUG_I("members: %p",members);
    //auto group_members = msg_GroupGetMembersListReq.group_members();
    //DEBUG_I("group_members: %p",group_members);
    //auto attach_data = msg_GroupGetMembersListReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupGetMembersListReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

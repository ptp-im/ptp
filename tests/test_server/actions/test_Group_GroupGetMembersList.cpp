#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupGetMembersListAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
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
    ImPdu pdu_GroupGetMembersListReq;
    pdu_GroupGetMembersListReq.SetPBMsg(&msg_GroupGetMembersListReq,CID_GroupGetMembersListReq,sep_no);
    CRequest request_GroupGetMembersListReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupGetMembersListReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupGetMembersListReq.GetHandle(),pMsgConn);
    request_GroupGetMembersListReq.SetRequestPdu(&pdu_GroupGetMembersListReq);
    ACTION_GROUP::GroupGetMembersListReqAction(&request_GroupGetMembersListReq);
    if(request_GroupGetMembersListReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupGetMembersListReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupGetMembersListReq.GetResponsePdu()->GetCommandId(),CID_GroupGetMembersListReq);
        ASSERT_EQ(request_GroupGetMembersListReq.IsNext(),true);

        CRequest request_next_GroupGetMembersListReq;
        request_next_GroupGetMembersListReq.SetIsBusinessConn(true);
        request_next_GroupGetMembersListReq.SetRequestPdu(request_GroupGetMembersListReq.GetResponsePdu());
        ACTION_GROUP::GroupGetMembersListReqAction(&request_next_GroupGetMembersListReq);

        PTP::Group::GroupGetMembersListRes msg_GroupGetMembersListRes;
        auto res = msg_GroupGetMembersListRes.ParseFromArray(request_next_GroupGetMembersListReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupGetMembersListReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupGetMembersListReq.GetResponsePdu()->GetCommandId(),CID_GroupGetMembersListRes);
        auto error = msg_GroupGetMembersListRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto group_members_updated_time = msg_GroupGetMembersListRes.group_members_updated_time();
        //DEBUG_I("group_members_updated_time: %d",group_members_updated_time);
        //auto group_id = msg_GroupGetMembersListRes.group_id();
        //DEBUG_I("group_id: %d",group_id);
        //auto members = msg_GroupGetMembersListRes.members();
        //DEBUG_I("members: %p",members);
        //auto group_members = msg_GroupGetMembersListRes.group_members();
        //DEBUG_I("group_members: %p",group_members);
        //auto attach_data = msg_GroupGetMembersListRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_GroupGetMembersListRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_GroupGetMembersListRes;
        request_GroupGetMembersListRes.SetIsBusinessConn(false);
        request_GroupGetMembersListRes.SetRequestPdu(request_next_GroupGetMembersListReq.GetResponsePdu());
        ACTION_GROUP::GroupGetMembersListResAction(&request_GroupGetMembersListRes);
        PTP::Group::GroupGetMembersListRes msg_final_GroupGetMembersListRes;
        res = msg_final_GroupGetMembersListRes.ParseFromArray(request_GroupGetMembersListRes.GetResponsePdu()->GetBodyData(), (int)request_GroupGetMembersListRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_GroupGetMembersListRes.GetResponsePdu()->GetCommandId(),CID_GroupGetMembersListRes);
        error = msg_final_GroupGetMembersListRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

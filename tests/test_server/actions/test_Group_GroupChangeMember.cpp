#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
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
    ImPdu pdu_GroupChangeMemberReq;
    pdu_GroupChangeMemberReq.SetPBMsg(&msg_GroupChangeMemberReq,CID_GroupChangeMemberReq,sep_no);
    CRequest request_GroupChangeMemberReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupChangeMemberReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupChangeMemberReq.GetHandle(),pMsgConn);
    request_GroupChangeMemberReq.SetRequestPdu(&pdu_GroupChangeMemberReq);
    ACTION_GROUP::GroupChangeMemberReqAction(&request_GroupChangeMemberReq);
    if(request_GroupChangeMemberReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupChangeMemberReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupChangeMemberReq.GetResponsePdu()->GetCommandId(),CID_GroupChangeMemberReq);
        ASSERT_EQ(request_GroupChangeMemberReq.IsNext(),true);

        CRequest request_next_GroupChangeMemberReq;
        request_next_GroupChangeMemberReq.SetIsBusinessConn(true);
        request_next_GroupChangeMemberReq.SetRequestPdu(request_GroupChangeMemberReq.GetResponsePdu());
        ACTION_GROUP::GroupChangeMemberReqAction(&request_next_GroupChangeMemberReq);

        PTP::Group::GroupChangeMemberRes msg_GroupChangeMemberRes;
        auto res = msg_GroupChangeMemberRes.ParseFromArray(request_next_GroupChangeMemberReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupChangeMemberReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupChangeMemberReq.GetResponsePdu()->GetCommandId(),CID_GroupChangeMemberRes);
        auto error = msg_GroupChangeMemberRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto group_member_modify_action = msg_GroupChangeMemberRes.group_member_modify_action();
        //DEBUG_I("group_member_modify_action: %p",group_member_modify_action);
        //auto group_id = msg_GroupChangeMemberRes.group_id();
        //DEBUG_I("group_id: %d",group_id);
        //auto members_chang = msg_GroupChangeMemberRes.members_chang();
        //DEBUG_I("members_chang: %p",members_chang);
        //auto attach_data = msg_GroupChangeMemberRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_GroupChangeMemberRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_GroupChangeMemberRes;
        request_GroupChangeMemberRes.SetIsBusinessConn(false);
        request_GroupChangeMemberRes.SetRequestPdu(request_next_GroupChangeMemberReq.GetResponsePdu());
        ACTION_GROUP::GroupChangeMemberResAction(&request_GroupChangeMemberRes);
        PTP::Group::GroupChangeMemberRes msg_final_GroupChangeMemberRes;
        res = msg_final_GroupChangeMemberRes.ParseFromArray(request_GroupChangeMemberRes.GetResponsePdu()->GetBodyData(), (int)request_GroupChangeMemberRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_GroupChangeMemberRes.GetResponsePdu()->GetCommandId(),CID_GroupChangeMemberRes);
        error = msg_final_GroupChangeMemberRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

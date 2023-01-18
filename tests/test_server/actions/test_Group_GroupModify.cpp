#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupModifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
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
    ImPdu pdu_GroupModifyReq;
    pdu_GroupModifyReq.SetPBMsg(&msg_GroupModifyReq,CID_GroupModifyReq,sep_no);
    CRequest request_GroupModifyReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupModifyReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupModifyReq.GetHandle(),pMsgConn);
    request_GroupModifyReq.SetRequestPdu(&pdu_GroupModifyReq);
    ACTION_GROUP::GroupModifyReqAction(&request_GroupModifyReq);
    if(request_GroupModifyReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupModifyReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupModifyReq.GetResponsePdu()->GetCommandId(),CID_GroupModifyReq);
        ASSERT_EQ(request_GroupModifyReq.IsNext(),true);

        CRequest request_next_GroupModifyReq;
        request_next_GroupModifyReq.SetIsBusinessConn(true);
        request_next_GroupModifyReq.SetRequestPdu(request_GroupModifyReq.GetResponsePdu());
        ACTION_GROUP::GroupModifyReqAction(&request_next_GroupModifyReq);

        PTP::Group::GroupModifyRes msg_GroupModifyRes;
        auto res = msg_GroupModifyRes.ParseFromArray(request_next_GroupModifyReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupModifyReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupModifyReq.GetResponsePdu()->GetCommandId(),CID_GroupModifyRes);
        auto error = msg_GroupModifyRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto group_modify_action = msg_GroupModifyRes.group_modify_action();
        //DEBUG_I("group_modify_action: %p",group_modify_action);
        //auto value = msg_GroupModifyRes.value();
        //DEBUG_I("value: %s",value.c_str());
        //auto group_id = msg_GroupModifyRes.group_id();
        //DEBUG_I("group_id: %d",group_id);
        //auto notify_members = msg_GroupModifyRes.notify_members();
        //DEBUG_I("notify_members: %p",notify_members);
        //auto attach_data = msg_GroupModifyRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_GroupModifyRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_GroupModifyRes;
        request_GroupModifyRes.SetIsBusinessConn(false);
        request_GroupModifyRes.SetRequestPdu(request_next_GroupModifyReq.GetResponsePdu());
        ACTION_GROUP::GroupModifyResAction(&request_GroupModifyRes);
        PTP::Group::GroupModifyRes msg_final_GroupModifyRes;
        res = msg_final_GroupModifyRes.ParseFromArray(request_GroupModifyRes.GetResponsePdu()->GetBodyData(), (int)request_GroupModifyRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_GroupModifyRes.GetResponsePdu()->GetCommandId(),CID_GroupModifyRes);
        error = msg_final_GroupModifyRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

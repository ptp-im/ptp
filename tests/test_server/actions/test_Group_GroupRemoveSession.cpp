#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupRemoveSessionAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupRemoveSessionAction) {
    test_int();
    PTP::Group::GroupRemoveSessionReq msg_GroupRemoveSessionReq;
    //msg_GroupRemoveSessionReq.set_group_id();
    //msg_GroupRemoveSessionReq.set_attach_data();
    //msg_GroupRemoveSessionReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_GroupRemoveSessionReq;
    pdu_GroupRemoveSessionReq.SetPBMsg(&msg_GroupRemoveSessionReq,CID_GroupRemoveSessionReq,sep_no);
    CRequest request_GroupRemoveSessionReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupRemoveSessionReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupRemoveSessionReq.GetHandle(),pMsgConn);
    request_GroupRemoveSessionReq.SetRequestPdu(&pdu_GroupRemoveSessionReq);
    ACTION_GROUP::GroupRemoveSessionReqAction(&request_GroupRemoveSessionReq);
    if(request_GroupRemoveSessionReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupRemoveSessionReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupRemoveSessionReq.GetResponsePdu()->GetCommandId(),CID_GroupRemoveSessionReq);
        ASSERT_EQ(request_GroupRemoveSessionReq.IsNext(),true);

        CRequest request_next_GroupRemoveSessionReq;
        request_next_GroupRemoveSessionReq.SetIsBusinessConn(true);
        request_next_GroupRemoveSessionReq.SetRequestPdu(request_GroupRemoveSessionReq.GetResponsePdu());
        ACTION_GROUP::GroupRemoveSessionReqAction(&request_next_GroupRemoveSessionReq);

        PTP::Group::GroupRemoveSessionRes msg_GroupRemoveSessionRes;
        auto res = msg_GroupRemoveSessionRes.ParseFromArray(request_next_GroupRemoveSessionReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupRemoveSessionReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupRemoveSessionReq.GetResponsePdu()->GetCommandId(),CID_GroupRemoveSessionRes);
        auto error = msg_GroupRemoveSessionRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto group_id = msg_GroupRemoveSessionRes.group_id();
        //DEBUG_I("group_id: %d",group_id);
        //auto attach_data = msg_GroupRemoveSessionRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_GroupRemoveSessionRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_GroupRemoveSessionRes;
        request_GroupRemoveSessionRes.SetIsBusinessConn(false);
        request_GroupRemoveSessionRes.SetRequestPdu(request_next_GroupRemoveSessionReq.GetResponsePdu());
        ACTION_GROUP::GroupRemoveSessionResAction(&request_GroupRemoveSessionRes);
        PTP::Group::GroupRemoveSessionRes msg_final_GroupRemoveSessionRes;
        res = msg_final_GroupRemoveSessionRes.ParseFromArray(request_GroupRemoveSessionRes.GetResponsePdu()->GetBodyData(), (int)request_GroupRemoveSessionRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_GroupRemoveSessionRes.GetResponsePdu()->GetCommandId(),CID_GroupRemoveSessionRes);
        error = msg_final_GroupRemoveSessionRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupRemoveSessionAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupRemoveSessionAction) {
    test_int();
    PTP::Group::GroupRemoveSessionReq msg_GroupRemoveSessionReq;
    //msg_GroupRemoveSessionReq.set_group_id();
    //msg_GroupRemoveSessionReq.set_attach_data();
    //msg_GroupRemoveSessionReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupRemoveSessionReq,CID_GroupRemoveSessionReq,sep_no);
    CRequest request_GroupRemoveSessionReq;
    CResponse response_GroupRemoveSessionReq;
    request_GroupRemoveSessionReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupRemoveSessionReq.GetHandle(),new CMsgSrvConn());
    request_GroupRemoveSessionReq.SetPdu(&pdu);
    ACTION_GROUP::GroupRemoveSessionReqAction(&request_GroupRemoveSessionReq,&response_GroupRemoveSessionReq);
    if(response_GroupRemoveSessionReq.GetPdu()){
        ASSERT_EQ(response_GroupRemoveSessionReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupRemoveSessionReq.GetPdu()->GetCommandId(),CID_GroupRemoveSessionRes);
    PTP::Group::GroupRemoveSessionRes msg_GroupRemoveSessionRes;
    auto res = msg_GroupRemoveSessionRes.ParseFromArray(response_GroupRemoveSessionReq.GetPdu()->GetBodyData(), (int)response_GroupRemoveSessionReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupRemoveSessionRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_id = msg_GroupRemoveSessionReq.group_id();
    //DEBUG_I("group_id: %d",group_id);
    //auto attach_data = msg_GroupRemoveSessionReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupRemoveSessionReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupModifyNotifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupModifyNotifyAction) {
    test_int();
    PTP::Group::GroupModifyNotify msg_GroupModifyNotify;
    //msg_GroupModifyNotify.set_group_modify_action();
    //msg_GroupModifyNotify.set_group_id();
    //msg_GroupModifyNotify.set_value();
    uint16_t sep_no = 101;
    ImPdu pdu_GroupModifyNotify;
    pdu_GroupModifyNotify.SetPBMsg(&msg_GroupModifyNotify,CID_GroupModifyNotify,sep_no);
    CRequest request_GroupModifyNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupModifyNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupModifyNotify.GetHandle(),pMsgConn);
    request_GroupModifyNotify.SetRequestPdu(&pdu_GroupModifyNotify);
    ACTION_GROUP::GroupModifyNotifyAction(&request_GroupModifyNotify);
    if(request_GroupModifyNotify.GetResponsePdu()){
        ASSERT_EQ(request_GroupModifyNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

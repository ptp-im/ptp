#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberNotifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberNotifyAction) {
    test_int();
    PTP::Group::GroupChangeMemberNotify msg_GroupChangeMemberNotify;
    //msg_GroupChangeMemberNotify.set_group_member_modify_action();
    //msg_GroupChangeMemberNotify.set_group_id();
    //msg_GroupChangeMemberNotify.set_members_chang();
    uint16_t sep_no = 101;
    ImPdu pdu_GroupChangeMemberNotify;
    pdu_GroupChangeMemberNotify.SetPBMsg(&msg_GroupChangeMemberNotify,CID_GroupChangeMemberNotify,sep_no);
    CRequest request_GroupChangeMemberNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupChangeMemberNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupChangeMemberNotify.GetHandle(),pMsgConn);
    request_GroupChangeMemberNotify.SetRequestPdu(&pdu_GroupChangeMemberNotify);
    ACTION_GROUP::GroupChangeMemberNotifyAction(&request_GroupChangeMemberNotify);
    if(request_GroupChangeMemberNotify.GetResponsePdu()){
        ASSERT_EQ(request_GroupChangeMemberNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberNotifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupChangeMemberNotify msg_GroupChangeMemberNotify;
    //msg_GroupChangeMemberNotify.set_group_member_modify_action();
    //msg_GroupChangeMemberNotify.set_group_id();
    //msg_GroupChangeMemberNotify.set_members_chang();
    uint16_t sep_no = 1;
    ImPdu pdu_GroupChangeMemberNotify;
    pdu_GroupChangeMemberNotify.SetPBMsg(&msg_GroupChangeMemberNotify,CID_GroupChangeMemberNotify,sep_no);
    CRequest request_GroupChangeMemberNotify;
    request_GroupChangeMemberNotify.SetHandle(pMsgConn->GetHandle());
    request_GroupChangeMemberNotify.SetRequestPdu(&pdu_GroupChangeMemberNotify);
    
    ACTION_GROUP::GroupChangeMemberNotifyAction(&request_GroupChangeMemberNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

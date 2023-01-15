#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberNotifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberNotifyAction) {
    test_int();
    PTP::Group::GroupChangeMemberNotify msg_GroupChangeMemberNotify;
    //msg_GroupChangeMemberNotify.set_group_member_modify_action();
    //msg_GroupChangeMemberNotify.set_group_id();
    //msg_GroupChangeMemberNotify.set_members_chang();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupChangeMemberNotify,CID_GroupChangeMemberNotify,sep_no);
    CRequest request_GroupChangeMemberNotify;
    CResponse response_GroupChangeMemberNotify;
    request_GroupChangeMemberNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupChangeMemberNotify.GetHandle(),new CMsgSrvConn());
    request_GroupChangeMemberNotify.SetPdu(&pdu);
    ACTION_GROUP::GroupChangeMemberNotifyAction(&request_GroupChangeMemberNotify,&response_GroupChangeMemberNotify);
    if(response_GroupChangeMemberNotify.GetPdu()){
        ASSERT_EQ(response_GroupChangeMemberNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

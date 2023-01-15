#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupModifyNotifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupModifyNotifyAction) {
    test_int();
    PTP::Group::GroupModifyNotify msg_GroupModifyNotify;
    //msg_GroupModifyNotify.set_group_modify_action();
    //msg_GroupModifyNotify.set_group_id();
    //msg_GroupModifyNotify.set_value();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupModifyNotify,CID_GroupModifyNotify,sep_no);
    CRequest request_GroupModifyNotify;
    CResponse response_GroupModifyNotify;
    request_GroupModifyNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupModifyNotify.GetHandle(),new CMsgSrvConn());
    request_GroupModifyNotify.SetPdu(&pdu);
    ACTION_GROUP::GroupModifyNotifyAction(&request_GroupModifyNotify,&response_GroupModifyNotify);
    if(response_GroupModifyNotify.GetPdu()){
        ASSERT_EQ(response_GroupModifyNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

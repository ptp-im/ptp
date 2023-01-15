#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupRemoveSessionNotifyAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupRemoveSessionNotifyAction) {
    test_int();
    PTP::Group::GroupRemoveSessionNotify msg_GroupRemoveSessionNotify;
    //msg_GroupRemoveSessionNotify.set_group_id();
    //msg_GroupRemoveSessionNotify.set_from_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupRemoveSessionNotify,CID_GroupRemoveSessionNotify,sep_no);
    CRequest request_GroupRemoveSessionNotify;
    CResponse response_GroupRemoveSessionNotify;
    request_GroupRemoveSessionNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupRemoveSessionNotify.GetHandle(),new CMsgSrvConn());
    request_GroupRemoveSessionNotify.SetPdu(&pdu);
    ACTION_GROUP::GroupRemoveSessionNotifyAction(&request_GroupRemoveSessionNotify,&response_GroupRemoveSessionNotify);
    if(response_GroupRemoveSessionNotify.GetPdu()){
        ASSERT_EQ(response_GroupRemoveSessionNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupRemoveSessionNotifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupRemoveSessionNotifyAction) {
    test_int();
    PTP::Group::GroupRemoveSessionNotify msg_GroupRemoveSessionNotify;
    //msg_GroupRemoveSessionNotify.set_group_id();
    //msg_GroupRemoveSessionNotify.set_from_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_GroupRemoveSessionNotify;
    pdu_GroupRemoveSessionNotify.SetPBMsg(&msg_GroupRemoveSessionNotify,CID_GroupRemoveSessionNotify,sep_no);
    CRequest request_GroupRemoveSessionNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupRemoveSessionNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupRemoveSessionNotify.GetHandle(),pMsgConn);
    request_GroupRemoveSessionNotify.SetRequestPdu(&pdu_GroupRemoveSessionNotify);
    ACTION_GROUP::GroupRemoveSessionNotifyAction(&request_GroupRemoveSessionNotify);
    if(request_GroupRemoveSessionNotify.GetResponsePdu()){
        ASSERT_EQ(request_GroupRemoveSessionNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

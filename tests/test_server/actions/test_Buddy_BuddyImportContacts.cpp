#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyImportContactsAction.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyImportContactsAction) {
    test_int();
    PTP::Buddy::BuddyImportContactsReq msg_BuddyImportContactsReq;
    //msg_BuddyImportContactsReq.set_phone_contacts();
    //msg_BuddyImportContactsReq.set_attach_data();
    //msg_BuddyImportContactsReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyImportContactsReq,CID_BuddyImportContactsReq,sep_no);
    CRequest request_BuddyImportContactsReq;
    CResponse response_BuddyImportContactsReq;
    request_BuddyImportContactsReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_BuddyImportContactsReq.GetHandle(),new CMsgSrvConn());
    request_BuddyImportContactsReq.SetPdu(&pdu);
    ACTION_BUDDY::BuddyImportContactsReqAction(&request_BuddyImportContactsReq,&response_BuddyImportContactsReq);
    if(response_BuddyImportContactsReq.GetPdu()){
        ASSERT_EQ(response_BuddyImportContactsReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_BuddyImportContactsReq.GetPdu()->GetCommandId(),CID_BuddyImportContactsRes);
    PTP::Buddy::BuddyImportContactsRes msg_BuddyImportContactsRes;
    auto res = msg_BuddyImportContactsRes.ParseFromArray(response_BuddyImportContactsReq.GetPdu()->GetBodyData(), (int)response_BuddyImportContactsReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_BuddyImportContactsRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto attach_data = msg_BuddyImportContactsReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_BuddyImportContactsReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyImportContactsAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyImportContactsAction) {
    test_int();
    PTP::Buddy::BuddyImportContactsReq msg_BuddyImportContactsReq;
    //msg_BuddyImportContactsReq.set_phone_contacts();
    //msg_BuddyImportContactsReq.set_attach_data();
    //msg_BuddyImportContactsReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_BuddyImportContactsReq;
    pdu_BuddyImportContactsReq.SetPBMsg(&msg_BuddyImportContactsReq,CID_BuddyImportContactsReq,sep_no);
    CRequest request_BuddyImportContactsReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyImportContactsReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyImportContactsReq.GetHandle(),pMsgConn);
    request_BuddyImportContactsReq.SetRequestPdu(&pdu_BuddyImportContactsReq);
    ACTION_BUDDY::BuddyImportContactsReqAction(&request_BuddyImportContactsReq);
    if(request_BuddyImportContactsReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyImportContactsReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyImportContactsReq.GetResponsePdu()->GetCommandId(),CID_BuddyImportContactsReq);
        ASSERT_EQ(request_BuddyImportContactsReq.IsNext(),true);

        CRequest request_next_BuddyImportContactsReq;
        request_next_BuddyImportContactsReq.SetIsBusinessConn(true);
        request_next_BuddyImportContactsReq.SetRequestPdu(request_BuddyImportContactsReq.GetResponsePdu());
        ACTION_BUDDY::BuddyImportContactsReqAction(&request_next_BuddyImportContactsReq);

        PTP::Buddy::BuddyImportContactsRes msg_BuddyImportContactsRes;
        auto res = msg_BuddyImportContactsRes.ParseFromArray(request_next_BuddyImportContactsReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyImportContactsReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_BuddyImportContactsReq.GetResponsePdu()->GetCommandId(),CID_BuddyImportContactsRes);
        auto error = msg_BuddyImportContactsRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto attach_data = msg_BuddyImportContactsRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_BuddyImportContactsRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_BuddyImportContactsRes;
        request_BuddyImportContactsRes.SetIsBusinessConn(false);
        request_BuddyImportContactsRes.SetRequestPdu(request_next_BuddyImportContactsReq.GetResponsePdu());
        ACTION_BUDDY::BuddyImportContactsResAction(&request_BuddyImportContactsRes);
        PTP::Buddy::BuddyImportContactsRes msg_final_BuddyImportContactsRes;
        res = msg_final_BuddyImportContactsRes.ParseFromArray(request_BuddyImportContactsRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyImportContactsRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_BuddyImportContactsRes.GetResponsePdu()->GetCommandId(),CID_BuddyImportContactsRes);
        error = msg_final_BuddyImportContactsRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

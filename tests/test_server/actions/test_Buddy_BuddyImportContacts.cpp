#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyImportContactsAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

TEST(test_Buddy, BuddyImportContactsAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Buddy::BuddyImportContactsReq msg_BuddyImportContactsReq;
    auto *phone_contacts = msg_BuddyImportContactsReq.add_phone_contacts();
    phone_contacts->set_first_name("first");
    phone_contacts->set_last_name("last");
    phone_contacts->set_phone("1212");
    phone_contacts->set_client_id((uint64_t)2222222222221);
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyImportContactsReq;
    pdu_BuddyImportContactsReq.SetPBMsg(&msg_BuddyImportContactsReq,CID_BuddyImportContactsReq,sep_no);
    CRequest request_BuddyImportContactsReq;
    request_BuddyImportContactsReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyImportContactsReq.SetRequestPdu(&pdu_BuddyImportContactsReq);
    
    ACTION_BUDDY::BuddyImportContactsReqAction(&request_BuddyImportContactsReq);
    
    if(request_BuddyImportContactsReq.HasNext()){
        auto pdu_next_BuddyImportContactsReq = request_BuddyImportContactsReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyImportContactsReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyImportContactsReq->GetCommandId(),CID_BuddyImportContactsReq);

        CRequest request_next_BuddyImportContactsReq;
        request_next_BuddyImportContactsReq.SetIsBusinessConn(true);
        request_next_BuddyImportContactsReq.SetRequestPdu(pdu_next_BuddyImportContactsReq);
        
        ACTION_BUDDY::BuddyImportContactsReqAction(&request_next_BuddyImportContactsReq);

        if(request_next_BuddyImportContactsReq.HasNext()){}
        if(request_next_BuddyImportContactsReq.GetResponsePdu()){
            PTP::Buddy::BuddyImportContactsRes msg_BuddyImportContactsRes;
            auto res = msg_BuddyImportContactsRes.ParseFromArray(request_next_BuddyImportContactsReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyImportContactsReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyImportContactsReq.GetResponsePdu()->GetCommandId(),CID_BuddyImportContactsRes);
            auto error = msg_BuddyImportContactsRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto auth_uid = msg_BuddyImportContactsRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_BuddyImportContactsRes;
            request_BuddyImportContactsRes.SetIsBusinessConn(false);
            request_BuddyImportContactsRes.SetHandle(pMsgConn->GetHandle());
            request_BuddyImportContactsRes.SetRequestPdu(request_next_BuddyImportContactsReq.GetResponsePdu());
            
            ACTION_BUDDY::BuddyImportContactsResAction(&request_BuddyImportContactsRes);
            
            if(request_BuddyImportContactsRes.HasNext()){}
            if(request_BuddyImportContactsRes.GetResponsePdu()){
                PTP::Buddy::BuddyImportContactsRes msg_final_BuddyImportContactsRes;
                res = msg_final_BuddyImportContactsRes.ParseFromArray(request_BuddyImportContactsRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyImportContactsRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_BuddyImportContactsRes.GetResponsePdu()->GetCommandId(),CID_BuddyImportContactsRes);
                error = msg_final_BuddyImportContactsRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

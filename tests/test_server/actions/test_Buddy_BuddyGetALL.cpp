#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyGetALLAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "PTP.Buddy.pb.h"
using namespace PTP::Common;

TEST(test_Buddy, ModelBuddy) {
    test_int();
    list<string> argv_users;
    CModelBuddy::getInstance()->handleUserInfoCacheArgv(argv_users,"1");
    ASSERT_EQ(true,true);
}

TEST(test_Buddy, BuddyGetALLAction) {
    test_int();
    PTP::Buddy::BuddyGetALLReq msg_BuddyGetALLReq;
    msg_BuddyGetALLReq.set_buddy_updated_time(time(nullptr));
    //msg_BuddyGetALLReq.set_attach_data();
    //msg_BuddyGetALLReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_BuddyGetALLReq,CID_BuddyGetALLReq,sep_no);
    CRequest request_BuddyGetALLReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_BuddyGetALLReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_BuddyGetALLReq.GetHandle(),pMsgConn);
    request_BuddyGetALLReq.SetRequestPdu(&pdu);
    ACTION_BUDDY::BuddyGetALLReqAction(&request_BuddyGetALLReq);
    if(request_BuddyGetALLReq.GetResponsePdu()){
        ASSERT_EQ(request_BuddyGetALLReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_BuddyGetALLReq.GetResponsePdu()->GetCommandId(),CID_BuddyGetALLReq);
        ASSERT_EQ(request_BuddyGetALLReq.IsNext(),true);

        CRequest request_BuddyGetALLReq1;
        request_BuddyGetALLReq1.SetIsBusinessConn(true);
        request_BuddyGetALLReq1.SetRequestPdu(request_BuddyGetALLReq.GetResponsePdu());
        ACTION_BUDDY::BuddyGetALLReqAction(&request_BuddyGetALLReq1);

        PTP::Buddy::BuddyGetALLRes msg_BuddyGetALLRes;
        auto res = msg_BuddyGetALLRes.ParseFromArray(request_BuddyGetALLReq1.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetALLReq1.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        auto error = msg_BuddyGetALLRes.error();
        ASSERT_EQ(error,NO_ERROR);

        auto buddy_updated_time = msg_BuddyGetALLRes.buddy_updated_time();
        DEBUG_I("buddy_updated_time: %d",buddy_updated_time);
        auto buddy_list = msg_BuddyGetALLRes.buddy_list();
        DEBUG_I("buddy_list: %p",buddy_list.data());
        auto auth_uid = msg_BuddyGetALLRes.auth_uid();
        DEBUG_I("auth_uid: %d",auth_uid);
        CRequest request_BuddyGetALLRes;

        request_BuddyGetALLRes.SetIsBusinessConn(false);
        request_BuddyGetALLRes.SetRequestPdu(request_BuddyGetALLReq1.GetResponsePdu());
        ACTION_BUDDY::BuddyGetALLResAction(&request_BuddyGetALLRes);
        PTP::Buddy::BuddyGetALLRes msg_BuddyGetALLRes1;
        res = msg_BuddyGetALLRes1.ParseFromArray(request_BuddyGetALLRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyGetALLRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        error = msg_BuddyGetALLRes1.error();
        ASSERT_EQ(error,NO_ERROR);

    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

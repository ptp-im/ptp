#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/ServerLoginAction.h"
#include "ptp_server/actions/models/ModelServer.h"
#include "PTP.Server.pb.h"

using namespace PTP::Common;

TEST(test_Server, ServerLoginAction) {
    test_int();
    PTP::Server::ServerLoginReq msg_ServerLoginReq;
    //msg_ServerLoginReq.set_address();
    //msg_ServerLoginReq.set_captcha();
    //msg_ServerLoginReq.set_sign();
    //msg_ServerLoginReq.set_attach_data();
    uint16_t sep_no = 101;
    ImPdu pdu_ServerLoginReq;
    pdu_ServerLoginReq.SetPBMsg(&msg_ServerLoginReq,CID_ServerLoginReq,sep_no);
    CRequest request_ServerLoginReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_ServerLoginReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_ServerLoginReq.GetHandle(),pMsgConn);
    request_ServerLoginReq.SetRequestPdu(&pdu_ServerLoginReq);
    ACTION_SERVER::ServerLoginReqAction(&request_ServerLoginReq);
    if(request_ServerLoginReq.GetResponsePdu()){
        ASSERT_EQ(request_ServerLoginReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_ServerLoginReq.GetResponsePdu()->GetCommandId(),CID_ServerLoginReq);
        ASSERT_EQ(request_ServerLoginReq.IsNext(),true);

        CRequest request_next_ServerLoginReq;
        request_next_ServerLoginReq.SetIsBusinessConn(true);
        request_next_ServerLoginReq.SetRequestPdu(request_ServerLoginReq.GetResponsePdu());
        ACTION_SERVER::ServerLoginReqAction(&request_next_ServerLoginReq);

        PTP::Server::ServerLoginRes msg_ServerLoginRes;
        auto res = msg_ServerLoginRes.ParseFromArray(request_next_ServerLoginReq.GetResponsePdu()->GetBodyData(), (int)request_next_ServerLoginReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_ServerLoginReq.GetResponsePdu()->GetCommandId(),CID_ServerLoginRes);
        auto error = msg_ServerLoginRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto user_info = msg_ServerLoginRes.user_info();
        //DEBUG_I("user_info: %p",user_info);
        //auto attach_data = msg_ServerLoginRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        
        CRequest request_ServerLoginRes;
        request_ServerLoginRes.SetIsBusinessConn(false);
        request_ServerLoginRes.SetRequestPdu(request_next_ServerLoginReq.GetResponsePdu());
        ACTION_SERVER::ServerLoginResAction(&request_ServerLoginRes);
        PTP::Server::ServerLoginRes msg_final_ServerLoginRes;
        res = msg_final_ServerLoginRes.ParseFromArray(request_ServerLoginRes.GetResponsePdu()->GetBodyData(), (int)request_ServerLoginRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_ServerLoginRes.GetResponsePdu()->GetCommandId(),CID_ServerLoginRes);
        error = msg_final_ServerLoginRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/CaptchaAction.h"
#include "ptp_server/actions/models/ModelOther.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, CaptchaAction) {
    test_int();
    PTP::Other::CaptchaReq msg_CaptchaReq;
    
    uint16_t sep_no = 101;
    ImPdu pdu_CaptchaReq;
    pdu_CaptchaReq.SetPBMsg(&msg_CaptchaReq,CID_CaptchaReq,sep_no);
    CRequest request_CaptchaReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_CaptchaReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_CaptchaReq.GetHandle(),pMsgConn);
    request_CaptchaReq.SetRequestPdu(&pdu_CaptchaReq);
    ACTION_OTHER::CaptchaReqAction(&request_CaptchaReq);
    if(request_CaptchaReq.GetResponsePdu()){
        ASSERT_EQ(request_CaptchaReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_CaptchaReq.GetResponsePdu()->GetCommandId(),CID_CaptchaReq);
        ASSERT_EQ(request_CaptchaReq.IsNext(),true);

        CRequest request_next_CaptchaReq;
        request_next_CaptchaReq.SetIsBusinessConn(true);
        request_next_CaptchaReq.SetRequestPdu(request_CaptchaReq.GetResponsePdu());
        ACTION_OTHER::CaptchaReqAction(&request_next_CaptchaReq);

        PTP::Other::CaptchaRes msg_CaptchaRes;
        auto res = msg_CaptchaRes.ParseFromArray(request_next_CaptchaReq.GetResponsePdu()->GetBodyData(), (int)request_next_CaptchaReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_CaptchaReq.GetResponsePdu()->GetCommandId(),CID_CaptchaRes);
        auto error = msg_CaptchaRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto captcha = msg_CaptchaRes.captcha();
        //DEBUG_I("captcha: %s",captcha.c_str());
        
        CRequest request_CaptchaRes;
        request_CaptchaRes.SetIsBusinessConn(false);
        request_CaptchaRes.SetRequestPdu(request_next_CaptchaReq.GetResponsePdu());
        ACTION_OTHER::CaptchaResAction(&request_CaptchaRes);
        PTP::Other::CaptchaRes msg_final_CaptchaRes;
        res = msg_final_CaptchaRes.ParseFromArray(request_CaptchaRes.GetResponsePdu()->GetBodyData(), (int)request_CaptchaRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_CaptchaRes.GetResponsePdu()->GetCommandId(),CID_CaptchaRes);
        error = msg_final_CaptchaRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

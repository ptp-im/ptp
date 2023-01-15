#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/CaptchaAction.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, CaptchaAction) {
    test_int();
    PTP::Other::CaptchaReq msg_CaptchaReq;
    
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_CaptchaReq,CID_CaptchaReq,sep_no);
    CRequest request_CaptchaReq;
    CResponse response_CaptchaReq;
    request_CaptchaReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_CaptchaReq.GetHandle(),new CMsgSrvConn());
    request_CaptchaReq.SetPdu(&pdu);
    ACTION_OTHER::CaptchaReqAction(&request_CaptchaReq,&response_CaptchaReq);
    if(response_CaptchaReq.GetPdu()){
        ASSERT_EQ(response_CaptchaReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_CaptchaReq.GetPdu()->GetCommandId(),CID_CaptchaRes);
    PTP::Other::CaptchaRes msg_CaptchaRes;
    auto res = msg_CaptchaRes.ParseFromArray(response_CaptchaReq.GetPdu()->GetBodyData(), (int)response_CaptchaReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_CaptchaRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto captcha = msg_CaptchaReq.captcha();
    //DEBUG_I("captcha: %s",captcha.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

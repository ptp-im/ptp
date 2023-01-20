#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/CaptchaAction.h"
#include "ptp_server/actions/models/ModelOther.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, CaptchaAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Other::CaptchaReq msg_CaptchaReq;
    
    uint16_t sep_no = 1;
    ImPdu pdu_CaptchaReq;
    pdu_CaptchaReq.SetPBMsg(&msg_CaptchaReq,CID_CaptchaReq,sep_no);
    CRequest request_CaptchaReq;
    request_CaptchaReq.SetHandle(pMsgConn->GetHandle());
    request_CaptchaReq.SetRequestPdu(&pdu_CaptchaReq);
    
    ACTION_OTHER::CaptchaReqAction(&request_CaptchaReq);
    
    if(request_CaptchaReq.HasNext()){
        auto pdu_next_CaptchaReq = request_CaptchaReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_CaptchaReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_CaptchaReq->GetCommandId(),CID_CaptchaReq);

        CRequest request_next_CaptchaReq;
        request_next_CaptchaReq.SetIsBusinessConn(true);
        request_next_CaptchaReq.SetRequestPdu(pdu_next_CaptchaReq);
        
        ACTION_OTHER::CaptchaReqAction(&request_next_CaptchaReq);

        if(request_next_CaptchaReq.HasNext()){}
        if(request_next_CaptchaReq.GetResponsePdu()){
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
            request_CaptchaRes.SetHandle(pMsgConn->GetHandle());
            request_CaptchaRes.SetRequestPdu(request_next_CaptchaReq.GetResponsePdu());
            
            ACTION_OTHER::CaptchaResAction(&request_CaptchaRes);
            
            if(request_CaptchaRes.HasNext()){}
            if(request_CaptchaRes.GetResponsePdu()){
                PTP::Other::CaptchaRes msg_final_CaptchaRes;
                res = msg_final_CaptchaRes.ParseFromArray(request_CaptchaRes.GetResponsePdu()->GetBodyData(), (int)request_CaptchaRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_CaptchaRes.GetResponsePdu()->GetCommandId(),CID_CaptchaRes);
                error = msg_final_CaptchaRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

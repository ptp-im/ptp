#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchPtpReq msg_SwitchPtpReq;
    //msg_SwitchPtpReq.set_to_adr();
    //msg_SwitchPtpReq.set_data();
    //msg_SwitchPtpReq.set_switch_type();
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchPtpReq;
    pdu_SwitchPtpReq.SetPBMsg(&msg_SwitchPtpReq,CID_SwitchPtpReq,sep_no);
    CRequest request_SwitchPtpReq;
    request_SwitchPtpReq.SetHandle(pMsgConn->GetHandle());
    request_SwitchPtpReq.SetRequestPdu(&pdu_SwitchPtpReq);
    
    ACTION_SWITCH::SwitchPtpReqAction(&request_SwitchPtpReq);
    
    if(request_SwitchPtpReq.HasNext()){
        auto pdu_next_SwitchPtpReq = request_SwitchPtpReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_SwitchPtpReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_SwitchPtpReq->GetCommandId(),CID_SwitchPtpReq);

        CRequest request_next_SwitchPtpReq;
        request_next_SwitchPtpReq.SetIsBusinessConn(true);
        request_next_SwitchPtpReq.SetRequestPdu(pdu_next_SwitchPtpReq);
        
        ACTION_SWITCH::SwitchPtpReqAction(&request_next_SwitchPtpReq);

        if(request_next_SwitchPtpReq.HasNext()){}
        if(request_next_SwitchPtpReq.GetResponsePdu()){
            PTP::Switch::SwitchPtpRes msg_SwitchPtpRes;
            auto res = msg_SwitchPtpRes.ParseFromArray(request_next_SwitchPtpReq.GetResponsePdu()->GetBodyData(), (int)request_next_SwitchPtpReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_SwitchPtpReq.GetResponsePdu()->GetCommandId(),CID_SwitchPtpRes);
            auto error = msg_SwitchPtpRes.error();
            ASSERT_EQ(error,NO_ERROR);
            
            
            CRequest request_SwitchPtpRes;
            request_SwitchPtpRes.SetIsBusinessConn(false);
            request_SwitchPtpRes.SetHandle(pMsgConn->GetHandle());
            request_SwitchPtpRes.SetRequestPdu(request_next_SwitchPtpReq.GetResponsePdu());
            
            ACTION_SWITCH::SwitchPtpResAction(&request_SwitchPtpRes);
            
            if(request_SwitchPtpRes.HasNext()){}
            if(request_SwitchPtpRes.GetResponsePdu()){
                PTP::Switch::SwitchPtpRes msg_final_SwitchPtpRes;
                res = msg_final_SwitchPtpRes.ParseFromArray(request_SwitchPtpRes.GetResponsePdu()->GetBodyData(), (int)request_SwitchPtpRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_SwitchPtpRes.GetResponsePdu()->GetCommandId(),CID_SwitchPtpRes);
                error = msg_final_SwitchPtpRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

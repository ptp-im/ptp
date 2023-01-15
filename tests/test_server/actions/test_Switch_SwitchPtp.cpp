#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpAction) {
    test_int();
    PTP::Switch::SwitchPtpReq msg_SwitchPtpReq;
    //msg_SwitchPtpReq.set_to_adr();
    //msg_SwitchPtpReq.set_data();
    //msg_SwitchPtpReq.set_switch_type();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_SwitchPtpReq,CID_SwitchPtpReq,sep_no);
    CRequest request_SwitchPtpReq;
    CResponse response_SwitchPtpReq;
    request_SwitchPtpReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_SwitchPtpReq.GetHandle(),new CMsgSrvConn());
    request_SwitchPtpReq.SetPdu(&pdu);
    ACTION_SWITCH::SwitchPtpReqAction(&request_SwitchPtpReq,&response_SwitchPtpReq);
    if(response_SwitchPtpReq.GetPdu()){
        ASSERT_EQ(response_SwitchPtpReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_SwitchPtpReq.GetPdu()->GetCommandId(),CID_SwitchPtpRes);
    PTP::Switch::SwitchPtpRes msg_SwitchPtpRes;
    auto res = msg_SwitchPtpRes.ParseFromArray(response_SwitchPtpReq.GetPdu()->GetBodyData(), (int)response_SwitchPtpReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_SwitchPtpRes.error();
    ASSERT_EQ(error,NO_ERROR);
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

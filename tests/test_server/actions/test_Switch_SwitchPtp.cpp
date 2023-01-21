#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchPtpReq msg_SwitchPtpReq;
    msg_SwitchPtpReq.set_to_adr("0x");
    msg_SwitchPtpReq.set_data("xxxx");
    msg_SwitchPtpReq.set_switch_type(PTP::Common::SwitchType_Answer);
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchPtpReq;
    pdu_SwitchPtpReq.SetPBMsg(&msg_SwitchPtpReq,CID_SwitchPtpReq,sep_no);
    CRequest request_SwitchPtpReq;
    request_SwitchPtpReq.SetHandle(pMsgConn->GetHandle());
    request_SwitchPtpReq.SetRequestPdu(&pdu_SwitchPtpReq);
    ACTION_SWITCH::SwitchPtpReqAction(&request_SwitchPtpReq);
    PTP::Switch::SwitchPtpRes msg_SwitchPtpRes;
    auto res = msg_SwitchPtpRes.ParseFromArray(request_SwitchPtpReq.GetResponsePdu()->GetBodyData(), (int)request_SwitchPtpReq.GetResponsePdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    ASSERT_EQ(request_SwitchPtpReq.GetResponsePdu()->GetCommandId(),CID_SwitchPtpRes);
    auto error = msg_SwitchPtpRes.error();
    DEBUG_D("error:%d",error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

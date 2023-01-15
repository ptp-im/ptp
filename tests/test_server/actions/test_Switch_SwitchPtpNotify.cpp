#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpNotifyAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpNotifyAction) {
    test_int();
    PTP::Switch::SwitchPtpNotify msg_SwitchPtpNotify;
    //msg_SwitchPtpNotify.set_from_adr();
    //msg_SwitchPtpNotify.set_data();
    //msg_SwitchPtpNotify.set_switch_type();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_SwitchPtpNotify,CID_SwitchPtpNotify,sep_no);
    CRequest request_SwitchPtpNotify;
    CResponse response_SwitchPtpNotify;
    request_SwitchPtpNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_SwitchPtpNotify.GetHandle(),new CMsgSrvConn());
    request_SwitchPtpNotify.SetPdu(&pdu);
    ACTION_SWITCH::SwitchPtpNotifyAction(&request_SwitchPtpNotify,&response_SwitchPtpNotify);
    if(response_SwitchPtpNotify.GetPdu()){
        ASSERT_EQ(response_SwitchPtpNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

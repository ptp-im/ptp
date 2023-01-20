#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpNotifyAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchPtpNotify msg_SwitchPtpNotify;
    //msg_SwitchPtpNotify.set_from_adr();
    //msg_SwitchPtpNotify.set_data();
    //msg_SwitchPtpNotify.set_switch_type();
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchPtpNotify;
    pdu_SwitchPtpNotify.SetPBMsg(&msg_SwitchPtpNotify,CID_SwitchPtpNotify,sep_no);
    CRequest request_SwitchPtpNotify;
    request_SwitchPtpNotify.SetHandle(pMsgConn->GetHandle());
    request_SwitchPtpNotify.SetRequestPdu(&pdu_SwitchPtpNotify);
    
    ACTION_SWITCH::SwitchPtpNotifyAction(&request_SwitchPtpNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

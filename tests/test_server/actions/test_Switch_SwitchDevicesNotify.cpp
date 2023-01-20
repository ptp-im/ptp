#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesNotifyAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchDevicesNotify msg_SwitchDevicesNotify;
    //msg_SwitchDevicesNotify.set_devices();
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchDevicesNotify;
    pdu_SwitchDevicesNotify.SetPBMsg(&msg_SwitchDevicesNotify,CID_SwitchDevicesNotify,sep_no);
    CRequest request_SwitchDevicesNotify;
    request_SwitchDevicesNotify.SetHandle(pMsgConn->GetHandle());
    request_SwitchDevicesNotify.SetRequestPdu(&pdu_SwitchDevicesNotify);
    
    ACTION_SWITCH::SwitchDevicesNotifyAction(&request_SwitchDevicesNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

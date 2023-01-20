#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchDevicesReq msg_SwitchDevicesReq;
    //msg_SwitchDevicesReq.set_browser_name();
    //msg_SwitchDevicesReq.set_browser_version();
    //msg_SwitchDevicesReq.set_os_name();
    //msg_SwitchDevicesReq.set_os_version();
    //msg_SwitchDevicesReq.set_is_intel();
    //msg_SwitchDevicesReq.set_client_id();
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchDevicesReq;
    pdu_SwitchDevicesReq.SetPBMsg(&msg_SwitchDevicesReq,CID_SwitchDevicesReq,sep_no);
    CRequest request_SwitchDevicesReq;
    request_SwitchDevicesReq.SetHandle(pMsgConn->GetHandle());
    request_SwitchDevicesReq.SetRequestPdu(&pdu_SwitchDevicesReq);
    
    ACTION_SWITCH::SwitchDevicesReqAction(&request_SwitchDevicesReq);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Switch::SwitchDevicesReq msg_SwitchDevicesReq;
    msg_SwitchDevicesReq.set_browser_name("chrome");
    msg_SwitchDevicesReq.set_browser_version("1.1.1");
    msg_SwitchDevicesReq.set_os_name("mac");
    msg_SwitchDevicesReq.set_os_version("1.1");
    msg_SwitchDevicesReq.set_is_intel(false);
    msg_SwitchDevicesReq.set_client_id("12121");
    uint16_t sep_no = 1;
    ImPdu pdu_SwitchDevicesReq;
    pdu_SwitchDevicesReq.SetPBMsg(&msg_SwitchDevicesReq,CID_SwitchDevicesReq,sep_no);
    CRequest request_SwitchDevicesReq;
    request_SwitchDevicesReq.SetHandle(pMsgConn->GetHandle());
    request_SwitchDevicesReq.SetRequestPdu(&pdu_SwitchDevicesReq);
    
    ACTION_SWITCH::SwitchDevicesReqAction(&request_SwitchDevicesReq);

    DEBUG_D("client_id=%s ", pMsgConn->m_client_id.c_str());
    DEBUG_D("browser_name=%s ", pMsgConn->m_browser_name.c_str());
    DEBUG_D("browser_version=%s ", pMsgConn->m_browser_version.c_str());
    DEBUG_D("os_name=%s ", pMsgConn->m_os_name.c_str());
    DEBUG_D("os_version=%s ", pMsgConn->m_os_version.c_str());
    DEBUG_D("is_intel=%d ", pMsgConn->m_is_intel);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

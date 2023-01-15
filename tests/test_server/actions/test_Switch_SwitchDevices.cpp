#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesAction) {
    test_int();
    PTP::Switch::SwitchDevicesReq msg_SwitchDevicesReq;
    //msg_SwitchDevicesReq.set_browser_name();
    //msg_SwitchDevicesReq.set_browser_version();
    //msg_SwitchDevicesReq.set_os_name();
    //msg_SwitchDevicesReq.set_os_version();
    //msg_SwitchDevicesReq.set_is_intel();
    //msg_SwitchDevicesReq.set_client_id();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_SwitchDevicesReq,CID_SwitchDevicesReq,sep_no);
    CRequest request_SwitchDevicesReq;
    CResponse response_SwitchDevicesReq;
    request_SwitchDevicesReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_SwitchDevicesReq.GetHandle(),new CMsgSrvConn());
    request_SwitchDevicesReq.SetPdu(&pdu);
    ACTION_SWITCH::SwitchDevicesReqAction(&request_SwitchDevicesReq,&response_SwitchDevicesReq);
    if(response_SwitchDevicesReq.GetPdu()){
        ASSERT_EQ(response_SwitchDevicesReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

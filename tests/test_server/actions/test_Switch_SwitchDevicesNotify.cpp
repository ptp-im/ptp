#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesNotifyAction.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesNotifyAction) {
    test_int();
    PTP::Switch::SwitchDevicesNotify msg_SwitchDevicesNotify;
    //msg_SwitchDevicesNotify.set_devices();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_SwitchDevicesNotify,CID_SwitchDevicesNotify,sep_no);
    CRequest request_SwitchDevicesNotify;
    CResponse response_SwitchDevicesNotify;
    request_SwitchDevicesNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_SwitchDevicesNotify.GetHandle(),new CMsgSrvConn());
    request_SwitchDevicesNotify.SetPdu(&pdu);
    ACTION_SWITCH::SwitchDevicesNotifyAction(&request_SwitchDevicesNotify,&response_SwitchDevicesNotify);
    if(response_SwitchDevicesNotify.GetPdu()){
        ASSERT_EQ(response_SwitchDevicesNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

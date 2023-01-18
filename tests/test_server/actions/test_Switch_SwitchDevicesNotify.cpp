#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesNotifyAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchDevicesNotifyAction) {
    test_int();
    PTP::Switch::SwitchDevicesNotify msg_SwitchDevicesNotify;
    //msg_SwitchDevicesNotify.set_devices();
    uint16_t sep_no = 101;
    ImPdu pdu_SwitchDevicesNotify;
    pdu_SwitchDevicesNotify.SetPBMsg(&msg_SwitchDevicesNotify,CID_SwitchDevicesNotify,sep_no);
    CRequest request_SwitchDevicesNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_SwitchDevicesNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_SwitchDevicesNotify.GetHandle(),pMsgConn);
    request_SwitchDevicesNotify.SetRequestPdu(&pdu_SwitchDevicesNotify);
    ACTION_SWITCH::SwitchDevicesNotifyAction(&request_SwitchDevicesNotify);
    if(request_SwitchDevicesNotify.GetResponsePdu()){
        ASSERT_EQ(request_SwitchDevicesNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

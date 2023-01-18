#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchDevicesAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
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
    ImPdu pdu_SwitchDevicesReq;
    pdu_SwitchDevicesReq.SetPBMsg(&msg_SwitchDevicesReq,CID_SwitchDevicesReq,sep_no);
    CRequest request_SwitchDevicesReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_SwitchDevicesReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_SwitchDevicesReq.GetHandle(),pMsgConn);
    request_SwitchDevicesReq.SetRequestPdu(&pdu_SwitchDevicesReq);
    ACTION_SWITCH::SwitchDevicesReqAction(&request_SwitchDevicesReq);
    if(request_SwitchDevicesReq.GetResponsePdu()){
        ASSERT_EQ(request_SwitchDevicesReq.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

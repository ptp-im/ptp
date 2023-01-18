#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/SwitchPtpNotifyAction.h"
#include "ptp_server/actions/models/ModelSwitch.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

TEST(test_Switch, SwitchPtpNotifyAction) {
    test_int();
    PTP::Switch::SwitchPtpNotify msg_SwitchPtpNotify;
    //msg_SwitchPtpNotify.set_from_adr();
    //msg_SwitchPtpNotify.set_data();
    //msg_SwitchPtpNotify.set_switch_type();
    uint16_t sep_no = 101;
    ImPdu pdu_SwitchPtpNotify;
    pdu_SwitchPtpNotify.SetPBMsg(&msg_SwitchPtpNotify,CID_SwitchPtpNotify,sep_no);
    CRequest request_SwitchPtpNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_SwitchPtpNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_SwitchPtpNotify.GetHandle(),pMsgConn);
    request_SwitchPtpNotify.SetRequestPdu(&pdu_SwitchPtpNotify);
    ACTION_SWITCH::SwitchPtpNotifyAction(&request_SwitchPtpNotify);
    if(request_SwitchPtpNotify.GetResponsePdu()){
        ASSERT_EQ(request_SwitchPtpNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

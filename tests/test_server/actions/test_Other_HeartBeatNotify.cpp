#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/HeartBeatNotifyAction.h"
#include "ptp_server/actions/models/ModelOther.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, HeartBeatNotifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Other::HeartBeatNotify msg_HeartBeatNotify;
    
    uint16_t sep_no = 1;
    ImPdu pdu_HeartBeatNotify;
    pdu_HeartBeatNotify.SetPBMsg(&msg_HeartBeatNotify,CID_HeartBeatNotify,sep_no);
    CRequest request_HeartBeatNotify;
    request_HeartBeatNotify.SetHandle(pMsgConn->GetHandle());
    request_HeartBeatNotify.SetRequestPdu(&pdu_HeartBeatNotify);
    
    ACTION_OTHER::HeartBeatNotifyAction(&request_HeartBeatNotify);
    
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

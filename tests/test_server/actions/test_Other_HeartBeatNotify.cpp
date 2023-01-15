#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/HeartBeatNotifyAction.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, HeartBeatNotifyAction) {
    test_int();
    PTP::Other::HeartBeatNotify msg_HeartBeatNotify;
    
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_HeartBeatNotify,CID_HeartBeatNotify,sep_no);
    CRequest request_HeartBeatNotify;
    CResponse response_HeartBeatNotify;
    request_HeartBeatNotify.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_HeartBeatNotify.GetHandle(),new CMsgSrvConn());
    request_HeartBeatNotify.SetPdu(&pdu);
    ACTION_OTHER::HeartBeatNotifyAction(&request_HeartBeatNotify,&response_HeartBeatNotify);
    if(response_HeartBeatNotify.GetPdu()){
        ASSERT_EQ(response_HeartBeatNotify.GetPdu()->GetSeqNum(),sep_no);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

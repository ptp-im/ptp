#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/HeartBeatNotifyAction.h"
#include "ptp_server/actions/models/ModelOther.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

TEST(test_Other, HeartBeatNotifyAction) {
    test_int();
    PTP::Other::HeartBeatNotify msg_HeartBeatNotify;
    
    uint16_t sep_no = 101;
    ImPdu pdu_HeartBeatNotify;
    pdu_HeartBeatNotify.SetPBMsg(&msg_HeartBeatNotify,CID_HeartBeatNotify,sep_no);
    CRequest request_HeartBeatNotify;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_HeartBeatNotify.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_HeartBeatNotify.GetHandle(),pMsgConn);
    request_HeartBeatNotify.SetRequestPdu(&pdu_HeartBeatNotify);
    ACTION_OTHER::HeartBeatNotifyAction(&request_HeartBeatNotify);
    if(request_HeartBeatNotify.GetResponsePdu()){
        ASSERT_EQ(request_HeartBeatNotify.GetResponsePdu()->GetSeqNum(),sep_no);
        
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

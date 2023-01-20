#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/AuthLogoutAction.h"
#include "PTP.Auth.pb.h"

using namespace PTP::Common;

TEST(test_Auth, AuthLogoutAction) {
    test_init();
    PTP::Auth::AuthLogoutReq msg;
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_AuthLogoutReq,sep_no);
    CRequest request;
    request.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request.GetHandle(),new CMsgSrvConn());
    request.SetRequestPdu(&pdu);
    ACTION_AUTH::AuthLogoutReqAction(&request);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

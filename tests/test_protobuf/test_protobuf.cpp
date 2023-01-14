#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/PTP.Common.pb.h"

//#include "ptp_protobuf/v2/PB.Command.Auth.pb.h"
//
//using namespace PB;
//using namespace PB::Command;

using namespace PTP::Common;

TEST(ptp_protobuf, Global_ERR) {
    DEBUG_D("ERR_NO:%x",E_REASON_NO_DB_SERVER);
//    DEBUG_D("ERR_AUTH_LOGIN_ERROR:%x",ERR_AUTH_LOGIN_ERROR);
//    ASSERT_EQ(ERR_NO,0x0);
    ASSERT_EQ(E_REASON_NO_DB_SERVER,7);
}


//
//TEST(ptp_protobuf, Global_ERR) {
//    DEBUG_D("ERR_NO:%x",ERR_NO);
//    DEBUG_D("ERR_AUTH_LOGIN_ERROR:%x",ERR_AUTH_LOGIN_ERROR);
//    ASSERT_EQ(ERR_NO,0x0);
//    ASSERT_EQ(ERR_AUTH_LOGIN_ERROR,0x500000);
//}
//
//TEST(ptp_protobuf, PB_User) {
//    AuthLoginResponse response;
//    response.set_error(ERR_NO);
//    PB::User *user = response.mutable_user();
//    user->set_is_online(true);
//    DEBUG_D("response error:%x",response.error());
//    DEBUG_D("response user is online:%d",response.user().is_online());
//}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

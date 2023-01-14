#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/actions/BuddyGetALLAction.h"
#include "PTP.Buddy.pb.h"
#include "PTP.Common.pb.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "conf/bd_server.conf"

using namespace PTP::Common;

TEST(test_Buddy, BuddyGetALLAction) {
    PTP::Buddy::BuddyGetALLReq msg;
    //msg.set_buddy_updated_time();
    //msg.set_auth_uid();
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_BuddyGetALLReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu->GetCommandId(),CID_BuddyGetALLRes);
    PTP::Buddy::BuddyGetALLRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength());
    ASSERT_EQ(res,true);
    //auto buddy_updated_time = msg_rsp.buddy_updated_time();
    //DEBUG_I("buddy_updated_time: %s",buddy_updated_time);
            
    //auto error = msg_rsp.error();
    //DEBUG_I("error: %s",error);
            
    //auto buddy_list = msg_rsp.buddy_list();
    //DEBUG_I("buddy_list: %s",buddy_list);
            
    //auto auth_uid = msg_rsp.auth_uid();
    //DEBUG_I("auth_uid: %s",auth_uid);
            
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

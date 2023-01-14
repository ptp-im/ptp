#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/actions/MsgReadAckAction.h"
#include "PTP.Msg.pb.h"
#include "PTP.Common.pb.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "conf/bd_server.conf"

using namespace PTP::Common;

TEST(test_Msg, MsgReadAckAction) {
    PTP::Msg::MsgReadAckReq msg;
    //msg.set_group_id();
    //msg.set_msg_id();
    //msg.set_auth_uid();
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_MsgReadAckReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

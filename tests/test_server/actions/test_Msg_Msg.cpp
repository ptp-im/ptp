#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/actions/MsgAction.h"
#include "PTP.Msg.pb.h"
#include "PTP.Common.pb.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "conf/bd_server.conf"

using namespace PTP::Common;

TEST(test_Msg, MsgAction) {
    PTP::Msg::MsgReq msg;
    //msg.set_group_adr();
    //msg.set_sent_at();
    //msg.set_msg_type();
    //msg.set_msg_data();
    //msg.set_auth_uid();
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_MsgReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu->GetCommandId(),CID_MsgRes);
    PTP::Msg::MsgRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength());
    ASSERT_EQ(res,true);
    //auto group_id = msg_rsp.group_id();
    //DEBUG_I("group_id: %s",group_id);
            
    //auto msg_id = msg_rsp.msg_id();
    //DEBUG_I("msg_id: %s",msg_id);
            
    //auto sent_at = msg_rsp.sent_at();
    //DEBUG_I("sent_at: %s",sent_at);
            
    //auto error = msg_rsp.error();
    //DEBUG_I("error: %s",error);
            
    //auto auth_uid = msg_rsp.auth_uid();
    //DEBUG_I("auth_uid: %s",auth_uid);
            
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

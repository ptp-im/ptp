#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/actions/GroupPreCreateAction.h"
#include "PTP.Group.pb.h"
#include "PTP.Common.pb.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "conf/bd_server.conf"

using namespace PTP::Common;

TEST(test_Group, GroupPreCreateAction) {
    PTP::Group::GroupPreCreateReq msg;
    //msg.set_group_type();
    //msg.set_members();
    //msg.set_auth_uid();
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_GroupPreCreateReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu->GetCommandId(),CID_GroupPreCreateRes);
    PTP::Group::GroupPreCreateRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength());
    ASSERT_EQ(res,true);
    //auto group_adr = msg_rsp.group_adr();
    //DEBUG_I("group_adr: %s",group_adr);
            
    //auto group_idx = msg_rsp.group_idx();
    //DEBUG_I("group_idx: %s",group_idx);
            
    //auto error = msg_rsp.error();
    //DEBUG_I("error: %s",error);
            
    //auto captcha = msg_rsp.captcha();
    //DEBUG_I("captcha: %s",captcha);
            
    //auto auth_uid = msg_rsp.auth_uid();
    //DEBUG_I("auth_uid: %s",auth_uid);
            
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

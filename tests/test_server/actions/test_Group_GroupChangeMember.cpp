#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_server/MsgSrvConn.h"
#include "ptp_server/CachePool.h"
#include "ptp_server/actions/GroupChangeMemberAction.h"
#include "PTP.Group.pb.h"
#include "PTP.Common.pb.h"

uint32_t accountId              = 1001;
#define CONFIG_PATH             "conf/bd_server.conf"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberAction) {
    PTP::Group::GroupChangeMemberReq msg;
    //msg.set_group_member_modify_action();
    //msg.set_group_id();
    //msg.set_members();
    //msg.set_auth_uid();
    CacheManager::setConfigPath(CONFIG_PATH);
    auto *pMsgSrvConn = new CMsgSrvConn();
    pMsgSrvConn->SetTest(true);
    pMsgSrvConn->SetHandle(100112);
    addMsgSrvConnByHandle(100112,pMsgSrvConn);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_GroupChangeMemberReq,0);
    pMsgSrvConn->HandlePdu(&pdu);
    auto pPdu = pMsgSrvConn->ReadTestPdu();
    ASSERT_EQ(pPdu->GetCommandId(),CID_GroupChangeMemberRes);
    PTP::Group::GroupChangeMemberRes msg_rsp;
    auto res = msg_rsp.ParseFromArray(pPdu->GetBodyData(), (int)pPdu->GetBodyLength());
    ASSERT_EQ(res,true);
    //auto group_member_modify_action = msg_rsp.group_member_modify_action();
    //DEBUG_I("group_member_modify_action: %s",group_member_modify_action);
            
    //auto group_id = msg_rsp.group_id();
    //DEBUG_I("group_id: %s",group_id);
            
    //auto members_chang = msg_rsp.members_chang();
    //DEBUG_I("members_chang: %s",members_chang);
            
    //auto error = msg_rsp.error();
    //DEBUG_I("error: %s",error);
            
    //auto auth_uid = msg_rsp.auth_uid();
    //DEBUG_I("auth_uid: %s",auth_uid);
            
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

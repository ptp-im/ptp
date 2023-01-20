#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "ptp_global/Helpers.h"
#include "ptp_protobuf/PTP.Common.pb.h"
#include "ptp_protobuf/PTP.Group.pb.h"
#include "ptp_protobuf/PTP.Buddy.pb.h"
#include "ptp_protobuf/ActionCommands.h"
#include "ImPdu.h"


TEST(ptp_protobuf, ActionCommands) {
    DEBUG_D("%s",getActionCommandsName(CID_AuthCaptchaReq).c_str());
}

TEST(ptp_protobuf, Global_ERR) {
    DEBUG_D("ERR_NO:%x",E_REASON_NO_DB_SERVER);
    ASSERT_EQ(E_REASON_NO_DB_SERVER,7);
}

TEST(ptp_protobuf, SetMsg) {
    PTP::Buddy::BuddyModifyNotify msg_rsp1;
    msg_rsp1.set_buddy_modify_action(PTP::Common::BuddyModifyAction_sign_info);
    msg_rsp1.set_value("SSSS");
    msg_rsp1.set_uid(1);
    ImPdu pdu1;
    pdu1.SetPBMsg(&msg_rsp1,CID_BuddyModifyNotify,102);
    pdu1.Dump();
    PTP::Buddy::BuddyGetALLReq msg_rsp2;
    msg_rsp2.set_buddy_updated_time(0);
    msg_rsp2.set_auth_uid(1);

    ImPdu pdu2;
    pdu2.SetPBMsg(&msg_rsp2,CID_BuddyGetALLReq,104);
    pdu2.SetReversed(1);
    pdu2.Dump();

    pdu2.SetPBMsg(&msg_rsp1);
    pdu2.SetReversed(0);
    pdu2.Dump();

    ASSERT_EQ(1,1);

}

TEST(ptp_protobuf, GroupCreateRes) {
    PTP::Group::GroupCreateRes msg_rsp;
    PTP::Group::GroupCreateRes msg_rsp1;
    msg_rsp.set_error(PTP::Common::E_SYSTEM);
    auto group = msg_rsp.mutable_group();
    uint32_t group_id = 1;
    group->set_group_id(group_id);
    group->set_group_adr("0xssss");
    group->set_about("about");
    group->set_name("name");
    group->set_avatar("avatar");
    group->set_updated_time(time(nullptr));
    group->set_created_time(time(nullptr));
    group->set_group_type(PTP::Common::GROUP_TYPE_MULTI);
    uint32_t memberUid = 99999+1;
    auto member = msg_rsp.add_group_members();
    member->set_uid(memberUid);
    member->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
    ImPdu pdu;
    pdu.SetPBMsg(&msg_rsp);
    auto* szData = new uchar_t[msg_rsp.ByteSizeLong()];
    msg_rsp.SerializeToArray(szData,msg_rsp.ByteSizeLong());
    DEBUG_D("msg_rsp size:%d", msg_rsp.ByteSizeLong());
    DEBUG_D("msg_rsp:%s", bytes_to_hex_string(szData,msg_rsp.ByteSizeLong()).c_str());

    delete[] szData;
    pdu.Dump();
    auto ret = msg_rsp1.ParseFromArray(pdu.GetBodyData(),pdu.GetBodyLength());
    DEBUG_D("ret: %d", ret);
    DEBUG_D("msg_rsp1 error: %d", msg_rsp1.error());
    DEBUG_D("msg_rsp1 size: %d", msg_rsp1.ByteSizeLong());
    ASSERT_EQ(msg_rsp1.error(),E_SYSTEM);
}

TEST(ptp_protobuf, GroupCreateReq) {
    PTP::Group::GroupPreCreateReq msg;
    PTP::Group::GroupPreCreateReq msg1;
    uint32_t memberUid = 99999+1;
    msg.set_group_type(PTP::Common::GROUP_TYPE_MULTI);
    msg.add_members(memberUid);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_GroupPreCreateReq,0);
    pdu.Dump();
    auto* szData = new uchar_t[msg.ByteSizeLong()];
    msg.SerializeToArray(szData,msg.ByteSizeLong());
    DEBUG_D("msg size:%d", msg.ByteSizeLong());
    DEBUG_D("msg:%s", bytes_to_hex_string(szData,msg.ByteSizeLong()).c_str());
    auto pdu1 = ImPdu::ReadPdu(pdu.GetBuffer()+0,pdu.GetLength());
    auto ret = msg1.ParseFromArray(pdu.GetBodyData(),pdu.GetBodyLength());
    DEBUG_D("ret: %d", ret);
    auto* szData1 = new uchar_t[msg1.ByteSizeLong()];
    msg1.SerializeToArray(szData1,msg1.ByteSizeLong());
    DEBUG_D("msg1 size:%d", msg1.ByteSizeLong());
    DEBUG_D("msg1:%s", bytes_to_hex_string(szData1,msg1.ByteSizeLong()).c_str());
}


TEST(ptp_protobuf, GroupPreCreateRes_E_GROUP_CREATE_PAIR_EXISTS) {
    PTP::Group::GroupCreateRes msg;
    msg.set_error(PTP::Common::E_GROUP_CREATE_PAIR_EXISTS);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_GroupPreCreateRes,0);
    pdu.Dump();
    PTP::Group::GroupCreateRes msg1;
    auto ret = msg1.ParseFromArray(pdu.GetBodyData(),pdu.GetBodyLength());
    DEBUG_D("ret: %d", ret);

    DEBUG_D("msg_rsp1 error: %d", msg1.error());
    DEBUG_D("msg_rsp1 size: %d", msg1.ByteSizeLong());

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

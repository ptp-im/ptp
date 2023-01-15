#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupGetListAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupGetListAction) {
    test_int();
    PTP::Group::GroupGetListReq msg_GroupGetListReq;
    //msg_GroupGetListReq.set_group_info_updated_time();
    //msg_GroupGetListReq.set_attach_data();
    //msg_GroupGetListReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupGetListReq,CID_GroupGetListReq,sep_no);
    CRequest request_GroupGetListReq;
    CResponse response_GroupGetListReq;
    request_GroupGetListReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupGetListReq.GetHandle(),new CMsgSrvConn());
    request_GroupGetListReq.SetPdu(&pdu);
    ACTION_GROUP::GroupGetListReqAction(&request_GroupGetListReq,&response_GroupGetListReq);
    if(response_GroupGetListReq.GetPdu()){
        ASSERT_EQ(response_GroupGetListReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupGetListReq.GetPdu()->GetCommandId(),CID_GroupGetListRes);
    PTP::Group::GroupGetListRes msg_GroupGetListRes;
    auto res = msg_GroupGetListRes.ParseFromArray(response_GroupGetListReq.GetPdu()->GetBodyData(), (int)response_GroupGetListReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupGetListRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_info_updated_time = msg_GroupGetListReq.group_info_updated_time();
    //DEBUG_I("group_info_updated_time: %d",group_info_updated_time);
    //auto groups = msg_GroupGetListReq.groups();
    //DEBUG_I("groups: %p",groups);
    //auto attach_data = msg_GroupGetListReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupGetListReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

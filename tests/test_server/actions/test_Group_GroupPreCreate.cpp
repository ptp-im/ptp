#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupPreCreateAction.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupPreCreateAction) {
    test_int();
    PTP::Group::GroupPreCreateReq msg_GroupPreCreateReq;
    //msg_GroupPreCreateReq.set_group_type();
    //msg_GroupPreCreateReq.set_members();
    //msg_GroupPreCreateReq.set_attach_data();
    //msg_GroupPreCreateReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu;
    pdu.SetPBMsg(&msg_GroupPreCreateReq,CID_GroupPreCreateReq,sep_no);
    CRequest request_GroupPreCreateReq;
    CResponse response_GroupPreCreateReq;
    request_GroupPreCreateReq.SetHandle(time(nullptr));
    addMsgSrvConnByHandle(request_GroupPreCreateReq.GetHandle(),new CMsgSrvConn());
    request_GroupPreCreateReq.SetPdu(&pdu);
    ACTION_GROUP::GroupPreCreateReqAction(&request_GroupPreCreateReq,&response_GroupPreCreateReq);
    if(response_GroupPreCreateReq.GetPdu()){
        ASSERT_EQ(response_GroupPreCreateReq.GetPdu()->GetSeqNum(),sep_no);
    }
    
    ASSERT_EQ(response_GroupPreCreateReq.GetPdu()->GetCommandId(),CID_GroupPreCreateRes);
    PTP::Group::GroupPreCreateRes msg_GroupPreCreateRes;
    auto res = msg_GroupPreCreateRes.ParseFromArray(response_GroupPreCreateReq.GetPdu()->GetBodyData(), (int)response_GroupPreCreateReq.GetPdu()->GetBodyLength());
    ASSERT_EQ(res,true);
    auto error = msg_GroupPreCreateRes.error();
    ASSERT_EQ(error,NO_ERROR);
    //auto group_adr = msg_GroupPreCreateReq.group_adr();
    //DEBUG_I("group_adr: %s",group_adr.c_str());
    //auto group_idx = msg_GroupPreCreateReq.group_idx();
    //DEBUG_I("group_idx: %d",group_idx);
    //auto captcha = msg_GroupPreCreateReq.captcha();
    //DEBUG_I("captcha: %s",captcha.c_str());
    //auto attach_data = msg_GroupPreCreateReq.attach_data();
    //DEBUG_I("attach_data: %s",attach_data.c_str());
    //auto auth_uid = msg_GroupPreCreateReq.auth_uid();
    //DEBUG_I("auth_uid: %d",auth_uid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

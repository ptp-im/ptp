#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupUnreadMsgAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupUnreadMsgAction) {
    test_int();
    PTP::Group::GroupUnreadMsgReq msg_GroupUnreadMsgReq;
    //msg_GroupUnreadMsgReq.set_attach_data();
    //msg_GroupUnreadMsgReq.set_auth_uid();
    uint16_t sep_no = 101;
    ImPdu pdu_GroupUnreadMsgReq;
    pdu_GroupUnreadMsgReq.SetPBMsg(&msg_GroupUnreadMsgReq,CID_GroupUnreadMsgReq,sep_no);
    CRequest request_GroupUnreadMsgReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    pMsgConn->SetUserId(99999+1);
    request_GroupUnreadMsgReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupUnreadMsgReq.GetHandle(),pMsgConn);
    request_GroupUnreadMsgReq.SetRequestPdu(&pdu_GroupUnreadMsgReq);
    ACTION_GROUP::GroupUnreadMsgReqAction(&request_GroupUnreadMsgReq);
    if(request_GroupUnreadMsgReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupUnreadMsgReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupUnreadMsgReq.GetResponsePdu()->GetCommandId(),CID_GroupUnreadMsgReq);
        ASSERT_EQ(request_GroupUnreadMsgReq.IsNext(),true);

        CRequest request_next_GroupUnreadMsgReq;
        request_next_GroupUnreadMsgReq.SetIsBusinessConn(true);
        request_next_GroupUnreadMsgReq.SetRequestPdu(request_GroupUnreadMsgReq.GetResponsePdu());
        ACTION_GROUP::GroupUnreadMsgReqAction(&request_next_GroupUnreadMsgReq);

        PTP::Group::GroupUnreadMsgRes msg_GroupUnreadMsgRes;
        auto res = msg_GroupUnreadMsgRes.ParseFromArray(request_next_GroupUnreadMsgReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupUnreadMsgReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupUnreadMsgReq.GetResponsePdu()->GetCommandId(),CID_GroupUnreadMsgRes);
        auto error = msg_GroupUnreadMsgRes.error();
        ASSERT_EQ(error,NO_ERROR);
        //auto unread_list = msg_GroupUnreadMsgRes.unread_list();
        //DEBUG_I("unread_list: %p",unread_list);
        //auto unread_cnt = msg_GroupUnreadMsgRes.unread_cnt();
        //DEBUG_I("unread_cnt: %d",unread_cnt);
        //auto attach_data = msg_GroupUnreadMsgRes.attach_data();
        //DEBUG_I("attach_data: %s",attach_data.c_str());
        //auto auth_uid = msg_GroupUnreadMsgRes.auth_uid();
        //DEBUG_I("auth_uid: %d",auth_uid);
        
        CRequest request_GroupUnreadMsgRes;
        request_GroupUnreadMsgRes.SetIsBusinessConn(false);
        request_GroupUnreadMsgRes.SetRequestPdu(request_next_GroupUnreadMsgReq.GetResponsePdu());
        ACTION_GROUP::GroupUnreadMsgResAction(&request_GroupUnreadMsgRes);
        PTP::Group::GroupUnreadMsgRes msg_final_GroupUnreadMsgRes;
        res = msg_final_GroupUnreadMsgRes.ParseFromArray(request_GroupUnreadMsgRes.GetResponsePdu()->GetBodyData(), (int)request_GroupUnreadMsgRes.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_GroupUnreadMsgRes.GetResponsePdu()->GetCommandId(),CID_GroupUnreadMsgRes);
        error = msg_final_GroupUnreadMsgRes.error();
        ASSERT_EQ(error,NO_ERROR);
    }
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

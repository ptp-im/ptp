#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupModifyAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupModifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupModifyReq msg_GroupModifyReq;
    //msg_GroupModifyReq.set_group_modify_action();
    //msg_GroupModifyReq.set_group_id();
    //msg_GroupModifyReq.set_value();
    uint16_t sep_no = 1;
    ImPdu pdu_GroupModifyReq;
    pdu_GroupModifyReq.SetPBMsg(&msg_GroupModifyReq,CID_GroupModifyReq,sep_no);
    CRequest request_GroupModifyReq;
    request_GroupModifyReq.SetHandle(pMsgConn->GetHandle());
    request_GroupModifyReq.SetRequestPdu(&pdu_GroupModifyReq);
    
    ACTION_GROUP::GroupModifyReqAction(&request_GroupModifyReq);
    
    if(request_GroupModifyReq.HasNext()){
        auto pdu_next_GroupModifyReq = request_GroupModifyReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupModifyReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupModifyReq->GetCommandId(),CID_GroupModifyReq);

        CRequest request_next_GroupModifyReq;
        request_next_GroupModifyReq.SetIsBusinessConn(true);
        request_next_GroupModifyReq.SetRequestPdu(pdu_next_GroupModifyReq);
        
        ACTION_GROUP::GroupModifyReqAction(&request_next_GroupModifyReq);

        if(request_next_GroupModifyReq.HasNext()){}
        if(request_next_GroupModifyReq.GetResponsePdu()){
            PTP::Group::GroupModifyRes msg_GroupModifyRes;
            auto res = msg_GroupModifyRes.ParseFromArray(request_next_GroupModifyReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupModifyReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupModifyReq.GetResponsePdu()->GetCommandId(),CID_GroupModifyRes);
            auto error = msg_GroupModifyRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_modify_action = msg_GroupModifyRes.group_modify_action();
            //DEBUG_I("group_modify_action: %p",group_modify_action);
            //auto value = msg_GroupModifyRes.value();
            //DEBUG_I("value: %s",value.c_str());
            //auto group_id = msg_GroupModifyRes.group_id();
            //DEBUG_I("group_id: %d",group_id);
            //auto notify_members = msg_GroupModifyRes.notify_members();
            //DEBUG_I("notify_members: %p",notify_members);
            //auto auth_uid = msg_GroupModifyRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupModifyRes;
            request_GroupModifyRes.SetIsBusinessConn(false);
            request_GroupModifyRes.SetHandle(pMsgConn->GetHandle());
            request_GroupModifyRes.SetRequestPdu(request_next_GroupModifyReq.GetResponsePdu());
            
            ACTION_GROUP::GroupModifyResAction(&request_GroupModifyRes);
            
            if(request_GroupModifyRes.HasNext()){}
            if(request_GroupModifyRes.GetResponsePdu()){
                PTP::Group::GroupModifyRes msg_final_GroupModifyRes;
                res = msg_final_GroupModifyRes.ParseFromArray(request_GroupModifyRes.GetResponsePdu()->GetBodyData(), (int)request_GroupModifyRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupModifyRes.GetResponsePdu()->GetCommandId(),CID_GroupModifyRes);
                error = msg_final_GroupModifyRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

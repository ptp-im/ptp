#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupChangeMemberAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupChangeMemberAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupChangeMemberReq msg_GroupChangeMemberReq;
    //msg_GroupChangeMemberReq.set_group_member_modify_action();
    //msg_GroupChangeMemberReq.set_group_id();
    //msg_GroupChangeMemberReq.set_members();
    uint16_t sep_no = 1;
    ImPdu pdu_GroupChangeMemberReq;
    pdu_GroupChangeMemberReq.SetPBMsg(&msg_GroupChangeMemberReq,CID_GroupChangeMemberReq,sep_no);
    CRequest request_GroupChangeMemberReq;
    request_GroupChangeMemberReq.SetHandle(pMsgConn->GetHandle());
    request_GroupChangeMemberReq.SetRequestPdu(&pdu_GroupChangeMemberReq);
    
    ACTION_GROUP::GroupChangeMemberReqAction(&request_GroupChangeMemberReq);
    
    if(request_GroupChangeMemberReq.HasNext()){
        auto pdu_next_GroupChangeMemberReq = request_GroupChangeMemberReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupChangeMemberReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupChangeMemberReq->GetCommandId(),CID_GroupChangeMemberReq);

        CRequest request_next_GroupChangeMemberReq;
        request_next_GroupChangeMemberReq.SetIsBusinessConn(true);
        request_next_GroupChangeMemberReq.SetRequestPdu(pdu_next_GroupChangeMemberReq);
        
        ACTION_GROUP::GroupChangeMemberReqAction(&request_next_GroupChangeMemberReq);

        if(request_next_GroupChangeMemberReq.HasNext()){}
        if(request_next_GroupChangeMemberReq.GetResponsePdu()){
            PTP::Group::GroupChangeMemberRes msg_GroupChangeMemberRes;
            auto res = msg_GroupChangeMemberRes.ParseFromArray(request_next_GroupChangeMemberReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupChangeMemberReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupChangeMemberReq.GetResponsePdu()->GetCommandId(),CID_GroupChangeMemberRes);
            auto error = msg_GroupChangeMemberRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_member_modify_action = msg_GroupChangeMemberRes.group_member_modify_action();
            //DEBUG_I("group_member_modify_action: %p",group_member_modify_action);
            //auto group_id = msg_GroupChangeMemberRes.group_id();
            //DEBUG_I("group_id: %d",group_id);
            //auto members_chang = msg_GroupChangeMemberRes.members_chang();
            //DEBUG_I("members_chang: %p",members_chang);
            //auto auth_uid = msg_GroupChangeMemberRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupChangeMemberRes;
            request_GroupChangeMemberRes.SetIsBusinessConn(false);
            request_GroupChangeMemberRes.SetHandle(pMsgConn->GetHandle());
            request_GroupChangeMemberRes.SetRequestPdu(request_next_GroupChangeMemberReq.GetResponsePdu());
            
            ACTION_GROUP::GroupChangeMemberResAction(&request_GroupChangeMemberRes);
            
            if(request_GroupChangeMemberRes.HasNext()){}
            if(request_GroupChangeMemberRes.GetResponsePdu()){
                PTP::Group::GroupChangeMemberRes msg_final_GroupChangeMemberRes;
                res = msg_final_GroupChangeMemberRes.ParseFromArray(request_GroupChangeMemberRes.GetResponsePdu()->GetBodyData(), (int)request_GroupChangeMemberRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupChangeMemberRes.GetResponsePdu()->GetCommandId(),CID_GroupChangeMemberRes);
                error = msg_final_GroupChangeMemberRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

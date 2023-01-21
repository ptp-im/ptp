#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupGetMembersListAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupGetMembersListAction) {
    auto *pMsgConn = test_init_msg_conn();
    auto groupType = PTP::Common::GROUP_TYPE_MULTI;
    createGroup(pMsgConn,groupType,pMsgConn->GetUserId(),pMsgConn->GetUserId()+1,DEFAULT_TEST_ACCOUNT_ID);
    auto group_id = get_test_default_group_id();

    PTP::Group::GroupGetMembersListReq msg_GroupGetMembersListReq;
    msg_GroupGetMembersListReq.set_group_members_updated_time(0);
    msg_GroupGetMembersListReq.set_group_id(group_id);
    uint16_t sep_no = 1;
    ImPdu pdu_GroupGetMembersListReq;
    pdu_GroupGetMembersListReq.SetPBMsg(&msg_GroupGetMembersListReq,CID_GroupGetMembersListReq,sep_no);
    CRequest request_GroupGetMembersListReq;
    request_GroupGetMembersListReq.SetHandle(pMsgConn->GetHandle());
    request_GroupGetMembersListReq.SetRequestPdu(&pdu_GroupGetMembersListReq);
    
    ACTION_GROUP::GroupGetMembersListReqAction(&request_GroupGetMembersListReq);
    
    if(request_GroupGetMembersListReq.HasNext()){
        auto pdu_next_GroupGetMembersListReq = request_GroupGetMembersListReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupGetMembersListReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupGetMembersListReq->GetCommandId(),CID_GroupGetMembersListReq);

        CRequest request_next_GroupGetMembersListReq;
        request_next_GroupGetMembersListReq.SetIsBusinessConn(true);
        request_next_GroupGetMembersListReq.SetRequestPdu(pdu_next_GroupGetMembersListReq);
        
        ACTION_GROUP::GroupGetMembersListReqAction(&request_next_GroupGetMembersListReq);

        if(request_next_GroupGetMembersListReq.HasNext()){}
        if(request_next_GroupGetMembersListReq.GetResponsePdu()){
            PTP::Group::GroupGetMembersListRes msg_GroupGetMembersListRes;
            auto res = msg_GroupGetMembersListRes.ParseFromArray(request_next_GroupGetMembersListReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupGetMembersListReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupGetMembersListReq.GetResponsePdu()->GetCommandId(),CID_GroupGetMembersListRes);
            auto error = msg_GroupGetMembersListRes.error();
            ASSERT_EQ(error,NO_ERROR);
            auto group_members_updated_time = msg_GroupGetMembersListRes.group_members_updated_time();
            DEBUG_I("group_members_updated_time: %d",group_members_updated_time);
            auto group_id = msg_GroupGetMembersListRes.group_id();
            DEBUG_I("group_id: %d",group_id);
            auto members = msg_GroupGetMembersListRes.members();
            DEBUG_I("members size: %d",members.size());
            auto group_members = msg_GroupGetMembersListRes.group_members();
            DEBUG_I("group_members size: %d",group_members.size());
            auto auth_uid = msg_GroupGetMembersListRes.auth_uid();
            DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupGetMembersListRes;
            request_GroupGetMembersListRes.SetIsBusinessConn(false);
            request_GroupGetMembersListRes.SetHandle(pMsgConn->GetHandle());
            request_GroupGetMembersListRes.SetRequestPdu(request_next_GroupGetMembersListReq.GetResponsePdu());
            
            ACTION_GROUP::GroupGetMembersListResAction(&request_GroupGetMembersListRes);
            
            if(request_GroupGetMembersListRes.HasNext()){}
            if(request_GroupGetMembersListRes.GetResponsePdu()){
                PTP::Group::GroupGetMembersListRes msg_final_GroupGetMembersListRes;
                res = msg_final_GroupGetMembersListRes.ParseFromArray(request_GroupGetMembersListRes.GetResponsePdu()->GetBodyData(), (int)request_GroupGetMembersListRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupGetMembersListRes.GetResponsePdu()->GetCommandId(),CID_GroupGetMembersListRes);
                error = msg_final_GroupGetMembersListRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

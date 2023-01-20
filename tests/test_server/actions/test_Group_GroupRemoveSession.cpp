#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupRemoveSessionAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupRemoveSessionAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupRemoveSessionReq msg_GroupRemoveSessionReq;
    //msg_GroupRemoveSessionReq.set_group_id();
    uint16_t sep_no = 1;
    ImPdu pdu_GroupRemoveSessionReq;
    pdu_GroupRemoveSessionReq.SetPBMsg(&msg_GroupRemoveSessionReq,CID_GroupRemoveSessionReq,sep_no);
    CRequest request_GroupRemoveSessionReq;
    request_GroupRemoveSessionReq.SetHandle(pMsgConn->GetHandle());
    request_GroupRemoveSessionReq.SetRequestPdu(&pdu_GroupRemoveSessionReq);
    
    ACTION_GROUP::GroupRemoveSessionReqAction(&request_GroupRemoveSessionReq);
    
    if(request_GroupRemoveSessionReq.HasNext()){
        auto pdu_next_GroupRemoveSessionReq = request_GroupRemoveSessionReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupRemoveSessionReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupRemoveSessionReq->GetCommandId(),CID_GroupRemoveSessionReq);

        CRequest request_next_GroupRemoveSessionReq;
        request_next_GroupRemoveSessionReq.SetIsBusinessConn(true);
        request_next_GroupRemoveSessionReq.SetRequestPdu(pdu_next_GroupRemoveSessionReq);
        
        ACTION_GROUP::GroupRemoveSessionReqAction(&request_next_GroupRemoveSessionReq);

        if(request_next_GroupRemoveSessionReq.HasNext()){}
        if(request_next_GroupRemoveSessionReq.GetResponsePdu()){
            PTP::Group::GroupRemoveSessionRes msg_GroupRemoveSessionRes;
            auto res = msg_GroupRemoveSessionRes.ParseFromArray(request_next_GroupRemoveSessionReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupRemoveSessionReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupRemoveSessionReq.GetResponsePdu()->GetCommandId(),CID_GroupRemoveSessionRes);
            auto error = msg_GroupRemoveSessionRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_id = msg_GroupRemoveSessionRes.group_id();
            //DEBUG_I("group_id: %d",group_id);
            //auto auth_uid = msg_GroupRemoveSessionRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupRemoveSessionRes;
            request_GroupRemoveSessionRes.SetIsBusinessConn(false);
            request_GroupRemoveSessionRes.SetHandle(pMsgConn->GetHandle());
            request_GroupRemoveSessionRes.SetRequestPdu(request_next_GroupRemoveSessionReq.GetResponsePdu());
            
            ACTION_GROUP::GroupRemoveSessionResAction(&request_GroupRemoveSessionRes);
            
            if(request_GroupRemoveSessionRes.HasNext()){}
            if(request_GroupRemoveSessionRes.GetResponsePdu()){
                PTP::Group::GroupRemoveSessionRes msg_final_GroupRemoveSessionRes;
                res = msg_final_GroupRemoveSessionRes.ParseFromArray(request_GroupRemoveSessionRes.GetResponsePdu()->GetBodyData(), (int)request_GroupRemoveSessionRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupRemoveSessionRes.GetResponsePdu()->GetCommandId(),CID_GroupRemoveSessionRes);
                error = msg_final_GroupRemoveSessionRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

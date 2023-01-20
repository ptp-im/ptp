#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupGetListAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

TEST(test_Group, GroupGetListAction) {
    auto *pMsgConn = test_init_msg_conn();
    PTP::Group::GroupGetListReq msg_GroupGetListReq;
    //msg_GroupGetListReq.set_group_info_updated_time();
    uint16_t sep_no = 1;
    ImPdu pdu_GroupGetListReq;
    pdu_GroupGetListReq.SetPBMsg(&msg_GroupGetListReq,CID_GroupGetListReq,sep_no);
    CRequest request_GroupGetListReq;
    request_GroupGetListReq.SetHandle(pMsgConn->GetHandle());
    request_GroupGetListReq.SetRequestPdu(&pdu_GroupGetListReq);
    
    ACTION_GROUP::GroupGetListReqAction(&request_GroupGetListReq);
    
    if(request_GroupGetListReq.HasNext()){
        auto pdu_next_GroupGetListReq = request_GroupGetListReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_GroupGetListReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_GroupGetListReq->GetCommandId(),CID_GroupGetListReq);

        CRequest request_next_GroupGetListReq;
        request_next_GroupGetListReq.SetIsBusinessConn(true);
        request_next_GroupGetListReq.SetRequestPdu(pdu_next_GroupGetListReq);
        
        ACTION_GROUP::GroupGetListReqAction(&request_next_GroupGetListReq);

        if(request_next_GroupGetListReq.HasNext()){}
        if(request_next_GroupGetListReq.GetResponsePdu()){
            PTP::Group::GroupGetListRes msg_GroupGetListRes;
            auto res = msg_GroupGetListRes.ParseFromArray(request_next_GroupGetListReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupGetListReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupGetListReq.GetResponsePdu()->GetCommandId(),CID_GroupGetListRes);
            auto error = msg_GroupGetListRes.error();
            ASSERT_EQ(error,NO_ERROR);
            //auto group_info_updated_time = msg_GroupGetListRes.group_info_updated_time();
            //DEBUG_I("group_info_updated_time: %d",group_info_updated_time);
            //auto groups = msg_GroupGetListRes.groups();
            //DEBUG_I("groups: %p",groups);
            //auto auth_uid = msg_GroupGetListRes.auth_uid();
            //DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_GroupGetListRes;
            request_GroupGetListRes.SetIsBusinessConn(false);
            request_GroupGetListRes.SetHandle(pMsgConn->GetHandle());
            request_GroupGetListRes.SetRequestPdu(request_next_GroupGetListReq.GetResponsePdu());
            
            ACTION_GROUP::GroupGetListResAction(&request_GroupGetListRes);
            
            if(request_GroupGetListRes.HasNext()){}
            if(request_GroupGetListRes.GetResponsePdu()){
                PTP::Group::GroupGetListRes msg_final_GroupGetListRes;
                res = msg_final_GroupGetListRes.ParseFromArray(request_GroupGetListRes.GetResponsePdu()->GetBodyData(), (int)request_GroupGetListRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupGetListRes.GetResponsePdu()->GetCommandId(),CID_GroupGetListRes);
                error = msg_final_GroupGetListRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

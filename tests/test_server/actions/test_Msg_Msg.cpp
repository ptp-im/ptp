#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/MsgAction.h"
#include "ptp_server/actions/MsgNotifyAction.h"
#include "ptp_server/actions/models/ModelMsg.h"
#include "ptp_net/AccountManager.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

TEST(test_Msg, MsgAction) {
    auto *pMsgConn = test_init_msg_conn();
    string group_adr = AccountManager::getInstance(DEFAULT_TEST_ACCOUNT_ID).getAccountGroupAddress(1);
    PTP::Msg::MsgReq msg_MsgReq;
    msg_MsgReq.set_group_adr(group_adr);
    msg_MsgReq.set_sent_at(time(nullptr));
    msg_MsgReq.set_msg_type(PTP::Common::MSG_TYPE_TEXT);
    msg_MsgReq.set_msg_data("SSSS");
    uint16_t sep_no = 1;
    ImPdu pdu_MsgReq;
    pdu_MsgReq.SetPBMsg(&msg_MsgReq,CID_MsgReq,sep_no);
    CRequest request_MsgReq;
    request_MsgReq.SetHandle(pMsgConn->GetHandle());
    request_MsgReq.SetRequestPdu(&pdu_MsgReq);
    
    ACTION_MSG::MsgReqAction(&request_MsgReq);
    
    if(request_MsgReq.HasNext()){
        auto pdu_next_MsgReq = request_MsgReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_MsgReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_MsgReq->GetCommandId(),CID_MsgReq);

        CRequest request_next_MsgReq;
        request_next_MsgReq.SetIsBusinessConn(true);
        request_next_MsgReq.SetRequestPdu(pdu_next_MsgReq);
        
        ACTION_MSG::MsgReqAction(&request_next_MsgReq);

        if(request_next_MsgReq.HasNext()){
            PTP::Msg::MsgNotify msg_MsgNotify;
            auto res = msg_MsgNotify.ParseFromArray(request_next_MsgReq.GetNextResponsePdu()->GetBodyData(), (int)request_next_MsgReq.GetNextResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_MsgReq.GetNextResponsePdu()->GetCommandId(),CID_MsgNotify);

            CRequest request_MsgNotify;
            request_MsgNotify.SetIsBusinessConn(true);
            request_MsgNotify.SetRequestPdu(request_next_MsgReq.GetNextResponsePdu());

            ACTION_MSG::MsgNotifyAction(&request_MsgNotify);
            if(request_MsgNotify.GetResponsePdu()){
                PTP::Msg::MsgNotify msg_MsgNotify1;
                res = msg_MsgNotify.ParseFromArray(request_MsgNotify.GetResponsePdu()->GetBodyData(), (int)request_MsgNotify.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_MsgNotify.GetResponsePdu()->GetCommandId(),CID_MsgNotify);

                CRequest request_MsgNotify1;
                request_MsgNotify1.SetIsBusinessConn(false);
                request_MsgNotify1.SetHandle(pMsgConn->GetHandle());
                request_MsgNotify1.SetRequestPdu(request_MsgNotify.GetResponsePdu());
                ACTION_MSG::MsgNotifyAction(&request_MsgNotify1);

            }
        }
        if(request_next_MsgReq.GetResponsePdu()){
            PTP::Msg::MsgRes msg_MsgRes;
            auto res = msg_MsgRes.ParseFromArray(request_next_MsgReq.GetResponsePdu()->GetBodyData(), (int)request_next_MsgReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_MsgReq.GetResponsePdu()->GetCommandId(),CID_MsgRes);
            auto error = msg_MsgRes.error();
            ASSERT_EQ(error,NO_ERROR);
            auto group_id = msg_MsgRes.group_id();
            DEBUG_I("group_id: %d",group_id);
            auto msg_id = msg_MsgRes.msg_id();
            DEBUG_I("msg_id: %d",msg_id);
            auto sent_at = msg_MsgRes.sent_at();
            DEBUG_I("sent_at: %d",sent_at);
            auto auth_uid = msg_MsgRes.auth_uid();
            DEBUG_I("auth_uid: %d",auth_uid);
            
            CRequest request_MsgRes;
            request_MsgRes.SetIsBusinessConn(false);
            request_MsgRes.SetHandle(pMsgConn->GetHandle());
            request_MsgRes.SetRequestPdu(request_next_MsgReq.GetResponsePdu());
            
            ACTION_MSG::MsgResAction(&request_MsgRes);

            if(request_MsgRes.HasNext()){}
            if(request_MsgRes.GetResponsePdu()){
                PTP::Msg::MsgRes msg_final_MsgRes;
                res = msg_final_MsgRes.ParseFromArray(request_MsgRes.GetResponsePdu()->GetBodyData(), (int)request_MsgRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_MsgRes.GetResponsePdu()->GetCommandId(),CID_MsgRes);
                error = msg_final_MsgRes.error();
                ASSERT_EQ(error,NO_ERROR);
            }
        }
      }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

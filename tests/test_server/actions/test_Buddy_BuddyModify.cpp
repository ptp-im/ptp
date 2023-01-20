#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/BuddyModifyAction.h"
#include "ptp_server/actions/BuddyModifyUpdatePairNotifyAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"

using namespace PTP::Common;

void buddyModify(CMsgSrvConn *pMsgConn,PTP::Common::BuddyModifyAction action,string value){
    PTP::Buddy::BuddyModifyReq msg_BuddyModifyReq;
    msg_BuddyModifyReq.set_buddy_modify_action(action);
    msg_BuddyModifyReq.set_value(value);
    uint16_t sep_no = 1;
    ImPdu pdu_BuddyModifyReq;
    pdu_BuddyModifyReq.SetPBMsg(&msg_BuddyModifyReq,CID_BuddyModifyReq,sep_no);
    CRequest request_BuddyModifyReq;
    request_BuddyModifyReq.SetHandle(pMsgConn->GetHandle());
    request_BuddyModifyReq.SetRequestPdu(&pdu_BuddyModifyReq);

    ACTION_BUDDY::BuddyModifyReqAction(&request_BuddyModifyReq);
    if(request_BuddyModifyReq.HasNext()){
        auto pdu_next_BuddyModifyReq = request_BuddyModifyReq.GetNextResponsePdu();
        ASSERT_EQ(pdu_next_BuddyModifyReq->GetSeqNum(),sep_no);
        ASSERT_EQ(pdu_next_BuddyModifyReq->GetCommandId(),CID_BuddyModifyReq);

        CRequest request_next_BuddyModifyReq;
        request_next_BuddyModifyReq.SetIsBusinessConn(true);
        request_next_BuddyModifyReq.SetRequestPdu(pdu_next_BuddyModifyReq);

        ACTION_BUDDY::BuddyModifyReqAction(&request_next_BuddyModifyReq);
        if(request_next_BuddyModifyReq.HasNext()){

        }
        if(request_next_BuddyModifyReq.GetResponsePdu()){
            PTP::Buddy::BuddyModifyRes msg_BuddyModifyRes;
            auto res = msg_BuddyModifyRes.ParseFromArray(request_next_BuddyModifyReq.GetResponsePdu()->GetBodyData(), (int)request_next_BuddyModifyReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_BuddyModifyReq.GetResponsePdu()->GetCommandId(),CID_BuddyModifyRes);
            DEBUG_I("error: %d",msg_BuddyModifyRes.error());

            auto error = msg_BuddyModifyRes.error();
            if(error == NO_ERROR){
                auto buddy_modify_action = msg_BuddyModifyRes.buddy_modify_action();
                DEBUG_I("buddy_modify_action: %d",buddy_modify_action);
                const auto& value1 = msg_BuddyModifyRes.value();
                DEBUG_I("value: %s",value1.c_str());
                ASSERT_EQ(value1,value);
                ASSERT_EQ(buddy_modify_action,action);

                CRequest request_BuddyModifyRes;
                request_BuddyModifyRes.SetIsBusinessConn(false);
                request_BuddyModifyRes.SetHandle(pMsgConn->GetHandle());
                request_BuddyModifyRes.SetRequestPdu(request_next_BuddyModifyReq.GetResponsePdu());

                ACTION_BUDDY::BuddyModifyResAction(&request_BuddyModifyRes);
                if(request_BuddyModifyRes.GetResponsePdu()){
                    PTP::Buddy::BuddyModifyRes msg_final_BuddyModifyRes;
                    res = msg_final_BuddyModifyRes.ParseFromArray(request_BuddyModifyRes.GetResponsePdu()->GetBodyData(), (int)request_BuddyModifyRes.GetResponsePdu()->GetBodyLength());
                    ASSERT_EQ(res,true);
                    ASSERT_EQ(request_BuddyModifyRes.GetResponsePdu()->GetCommandId(),CID_BuddyModifyRes);
                    error = msg_final_BuddyModifyRes.error();
                    ASSERT_EQ(error,NO_ERROR);
                }
                if(request_BuddyModifyRes.HasNext()){
                    PTP::Buddy::BuddyModifyUpdatePairNotify msg_BuddyModifyUpdatePairNotify;
                    res = msg_BuddyModifyUpdatePairNotify.ParseFromArray(request_BuddyModifyRes.GetNextResponsePdu()->GetBodyData(), (int)request_BuddyModifyRes.GetNextResponsePdu()->GetBodyLength());
                    ASSERT_EQ(res,true);
                    ASSERT_EQ(request_BuddyModifyRes.GetNextResponsePdu()->GetCommandId(),CID_BuddyModifyUpdatePairNotify);
                    for(uint32_t user_id:msg_BuddyModifyUpdatePairNotify.pair_uid_list()){
                        DEBUG_D("pair_uid_list %d",user_id);
                    }

                    ImPdu pdu_BuddyModifyUpdatePairNotify;
                    pdu_BuddyModifyUpdatePairNotify.SetPBMsg(&msg_BuddyModifyUpdatePairNotify,CID_BuddyModifyUpdatePairNotify,sep_no);
                    CRequest request_BuddyModifyUpdatePairNotify;
                    request_BuddyModifyUpdatePairNotify.SetHandle(pMsgConn->GetHandle());
                    request_BuddyModifyUpdatePairNotify.SetRequestPdu(&pdu_BuddyModifyUpdatePairNotify);
                    request_BuddyModifyUpdatePairNotify.SetIsBusinessConn(true);

                    ACTION_BUDDY::BuddyModifyUpdatePairNotifyAction(&request_BuddyModifyUpdatePairNotify);

                }

            }
        }

    }
}

TEST(test_Buddy, BuddyModifyAction) {
    auto *pMsgConn = test_init_msg_conn();
    pMsgConn->SetOpen(true);
    auto action = PTP::Common::BuddyModifyAction_user_name;
    string value = "username12222";
    buddyModify(pMsgConn,action,value);
    action = PTP::Common::BuddyModifyAction_nickname;
    value = "nickname";
    buddyModify(pMsgConn,action,value);
    action = PTP::Common::BuddyModifyAction_first_name;
    value = "first";
    buddyModify(pMsgConn,action,value);
    action = PTP::Common::BuddyModifyAction_last_name;
    value = "last";
    buddyModify(pMsgConn,action,value);
    action = PTP::Common::BuddyModifyAction_sign_info;
    value = "sign info";
    buddyModify(pMsgConn,action,value);
    action = PTP::Common::BuddyModifyAction_avatar;
    value = "avatar";
    buddyModify(pMsgConn,action,value);

    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
    UserInfo userInfo;
    CModelBuddy::getUserInfo(pCacheConn,&userInfo,pMsgConn->GetUserId());
    DEBUG_D("pMsgConn uid:%d",pMsgConn->GetUserId());
    DEBUG_D("userInfo uid:%d",userInfo.uid());
    DEBUG_D("avatar:%s",userInfo.avatar().c_str());
    DEBUG_D("user_name:%s",userInfo.user_name().c_str());
    DEBUG_D("sign_info:%s",userInfo.sign_info().c_str());
    DEBUG_D("first_name:%s",userInfo.first_name().c_str());
    DEBUG_D("last_name:%s",userInfo.last_name().c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

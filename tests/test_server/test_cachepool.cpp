#include <gtest/gtest.h>

#include "ptp_global/Logger.h"
#include "test_init.h"

#include "ptp_server/actions/AuthLoginAction.h"
#include "ptp_server/actions/AuthCaptchaAction.h"
#include "ptp_server/actions/ServerLoginAction.h"
#include "ptp_server/actions/models/ModelBuddy.h"
#include "ptp_net/AccountManager.h"
#include "PTP.Auth.pb.h"
#include "PTP.Server.pb.h"
#include "ptp_server/ImUser.h"

#include "ptp_server/actions/GroupCreateAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "ptp_server/actions/GroupPreCreateAction.h"

TEST(cachepool, cachepool) {
    test_init();
    CacheManager* pCacheManager = CacheManager::getInstance();
    ASSERT_TRUE(pCacheManager != NULL);
}

void createGroup1(CMsgSrvConn *pMsgConn, PTP::Common::GroupType groupType, uint32_t fromUid, uint32_t toUid, uint32_t accountId){
    DEBUG_D("createGroup ========================================>>>");
    string captcha;
    uint32_t group_idx = 0;
    uint32_t group_id = 0;

    PTP::Group::GroupPreCreateReq msg_GroupPreCreateReq;
    msg_GroupPreCreateReq.set_group_type(groupType);
    msg_GroupPreCreateReq.add_members(toUid);

    uint16_t sep_no = time(nullptr);
    ImPdu pdu_GroupPreCreateReq;
    pdu_GroupPreCreateReq.SetPBMsg(&msg_GroupPreCreateReq,CID_GroupPreCreateReq,sep_no);
    CRequest request_GroupPreCreateReq;
    request_GroupPreCreateReq.SetHandle(pMsgConn->GetHandle());
    request_GroupPreCreateReq.SetRequestPdu(&pdu_GroupPreCreateReq);
    ACTION_GROUP::GroupPreCreateReqAction(&request_GroupPreCreateReq);
    if(request_GroupPreCreateReq.HasNext()){
        auto request_GroupPreCreateReq_next_pdu = request_GroupPreCreateReq.GetNextResponsePdu();
        ASSERT_EQ(request_GroupPreCreateReq_next_pdu->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupPreCreateReq_next_pdu->GetCommandId(),CID_GroupPreCreateReq);

        CRequest request_next_GroupPreCreateReq;
        request_next_GroupPreCreateReq.SetIsBusinessConn(true);
        request_next_GroupPreCreateReq.SetRequestPdu(request_GroupPreCreateReq_next_pdu);
        ACTION_GROUP::GroupPreCreateReqAction(&request_next_GroupPreCreateReq);
        if(request_next_GroupPreCreateReq.GetResponsePdu()){
            auto request_next_GroupPreCreateReq_next_pdu = request_next_GroupPreCreateReq.GetResponsePdu();
            PTP::Group::GroupPreCreateRes msg_GroupPreCreateRes;
            auto res = msg_GroupPreCreateRes.ParseFromArray(request_next_GroupPreCreateReq_next_pdu->GetBodyData(), (int)request_next_GroupPreCreateReq_next_pdu->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupPreCreateReq_next_pdu->GetCommandId(),CID_GroupPreCreateRes);
            auto error = msg_GroupPreCreateRes.error();
            ASSERT_EQ(error,NO_ERROR);
            auto group_adr = msg_GroupPreCreateRes.group_adr();
            DEBUG_D("group_adr: %s",group_adr.c_str());
            auto group_idx1 = msg_GroupPreCreateRes.group_idx();
            DEBUG_D("group_idx: %d",group_idx1);
            auto captcha1 = msg_GroupPreCreateRes.captcha();
            DEBUG_D("captcha: %s",captcha1.c_str());
            auto auth_uid = msg_GroupPreCreateRes.auth_uid();
            DEBUG_D("auth_uid: %d",auth_uid);
            ASSERT_EQ(group_idx1> 0,true);
            ASSERT_EQ(captcha1,"");

            CRequest request_GroupPreCreateRes;
            request_GroupPreCreateRes.SetIsBusinessConn(false);
            DEBUG_D("Handle:%d",pMsgConn->GetHandle());
            request_GroupPreCreateRes.SetHandle(pMsgConn->GetHandle());
            request_GroupPreCreateRes.SetRequestPdu(request_next_GroupPreCreateReq_next_pdu);
            ACTION_GROUP::GroupPreCreateResAction(&request_GroupPreCreateRes);
            if(request_GroupPreCreateRes.GetResponsePdu()){
                PTP::Group::GroupPreCreateRes msg_final_GroupPreCreateRes;
                res = msg_final_GroupPreCreateRes.ParseFromArray(request_GroupPreCreateRes.GetResponsePdu()->GetBodyData(), (int)request_GroupPreCreateRes.GetResponsePdu()->GetBodyLength());
                ASSERT_EQ(res,true);
                ASSERT_EQ(request_GroupPreCreateRes.GetResponsePdu()->GetCommandId(),CID_GroupPreCreateRes);
                error = msg_final_GroupPreCreateRes.error();
                ASSERT_EQ(error,NO_ERROR);
                group_idx = msg_final_GroupPreCreateRes.group_idx();
                ASSERT_EQ(group_idx,group_idx1);
                captcha = msg_final_GroupPreCreateRes.captcha();
                ASSERT_EQ(captcha,pMsgConn->GetCaptcha());
                DEBUG_D("captcha: %s",captcha1.c_str());
            }
        }

    }
    if(group_idx > 0){
        PTP::Group::GroupCreateReq msg_GroupCreateReq;
        unsigned char sign65[65];
        AccountManager::getInstance(accountId).signGroupMessage(captcha,(int32_t)group_idx,sign65);
        msg_GroupCreateReq.set_sign(sign65,65);
        msg_GroupCreateReq.set_group_idx(group_idx);
        msg_GroupCreateReq.set_captcha(captcha);
        msg_GroupCreateReq.set_group_type(groupType);
        msg_GroupCreateReq.set_name("name");
        msg_GroupCreateReq.set_avatar("avatar");
        msg_GroupCreateReq.add_members(toUid);
        msg_GroupCreateReq.set_about("about");
        ImPdu pdu_GroupCreateReq;
        pdu_GroupCreateReq.SetPBMsg(&msg_GroupCreateReq,CID_GroupCreateReq,sep_no);
        CRequest request_GroupCreateReq;
        request_GroupCreateReq.SetHandle(pMsgConn->GetHandle());
        addMsgSrvConnByHandle(request_GroupCreateReq.GetHandle(),pMsgConn);
        request_GroupCreateReq.SetRequestPdu(&pdu_GroupCreateReq);
        ACTION_GROUP::GroupCreateReqAction(&request_GroupCreateReq);
        if(request_GroupCreateReq.HasNext()){
            auto pdu_next_GroupCreateReq = request_GroupCreateReq.GetNextResponsePdu();
            ASSERT_EQ(pdu_next_GroupCreateReq->GetSeqNum(),sep_no);
            ASSERT_EQ(pdu_next_GroupCreateReq->GetCommandId(),CID_GroupCreateReq);

            CRequest request_next_GroupCreateReq;
            request_next_GroupCreateReq.SetIsBusinessConn(true);
            request_next_GroupCreateReq.SetRequestPdu(pdu_next_GroupCreateReq);

            ACTION_GROUP::GroupCreateReqAction(&request_next_GroupCreateReq);

            PTP::Group::GroupCreateRes msg_GroupCreateRes;
            auto res = msg_GroupCreateRes.ParseFromArray(request_next_GroupCreateReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupCreateReq.GetResponsePdu()->GetBodyLength());
            request_next_GroupCreateReq.GetResponsePdu()->Dump();
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupCreateReq.GetResponsePdu()->GetCommandId(),CID_GroupCreateRes);
            auto error = msg_GroupCreateRes.error();
            if(error == PTP::Common::E_GROUP_CREATE_PAIR_EXISTS){
                DEBUG_D("E_GROUP_CREATE_PAIR_EXISTS");
                return;
            }
            if(error == PTP::Common::E_GROUP_HAS_CREATED){
                DEBUG_D("E_GROUP_HAS_CREATED");
                return;
            }
            DEBUG_D("error:%d",error);
            ASSERT_EQ(error,NO_ERROR);
            ASSERT_EQ(fromUid,msg_GroupCreateRes.auth_uid());
            DEBUG_D("auth_uid: %d",msg_GroupCreateRes.auth_uid());
            auto group = msg_GroupCreateRes.group();
            DEBUG_D("group_id: %d",group.group_id());
            DEBUG_D("group_idx: %d",group.group_idx());
            DEBUG_D("group_type: %d",group.group_type());
            DEBUG_D("group_adr: %s",group.group_adr().c_str());
            auto group_members = msg_GroupCreateRes.group_members();
            DEBUG_D("group_members size: %d",group_members.size());
            group_id = group.group_id();
            for (const auto& member: group_members) {
                DEBUG_D("===>>member uid: %d",member.uid());
                DEBUG_D("member member_status: %d",member.member_status());
            }

            CacheManager *pCacheManager = CacheManager::getInstance();
            CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
            GroupInfo groupInfoFromCache;
            CModelGroup::getGroupInfoById(pCacheConn,&groupInfoFromCache,group.group_id());
            ASSERT_EQ(groupInfoFromCache.group_id(),msg_GroupCreateRes.group().group_id());
            ASSERT_EQ(groupInfoFromCache.group_idx(),msg_GroupCreateRes.group().group_idx());
            ASSERT_EQ(groupInfoFromCache.group_adr(),msg_GroupCreateRes.group().group_adr());
            ASSERT_EQ(groupInfoFromCache.about(),msg_GroupCreateRes.group().about());
            ASSERT_EQ(groupInfoFromCache.name(),msg_GroupCreateRes.group().name());
            ASSERT_EQ(groupInfoFromCache.avatar(),msg_GroupCreateRes.group().avatar());
            ASSERT_EQ(groupInfoFromCache.pair_uid(),msg_GroupCreateRes.group().pair_uid());
            ASSERT_EQ(groupInfoFromCache.owner_uid(),msg_GroupCreateRes.group().owner_uid());

            ASSERT_EQ(fromUid,msg_GroupCreateRes.group().owner_uid());
            if(groupType == PTP::Common::GROUP_TYPE_PAIR){
                ASSERT_EQ(group.group_id(),CModelGroup::getGroupPairRelation(pCacheConn,fromUid,toUid));

                list<string> pair_user_ids;
                CModelGroup::getUserGroupPairs(pCacheConn,pair_user_ids,fromUid);

                ASSERT_TRUE(list_string_contains(pair_user_ids, to_string(toUid)));

                list<string> pair_user_ids1;
                CModelGroup::getUserGroupPairs(pCacheConn,pair_user_ids1,toUid);
                ASSERT_TRUE(list_string_contains(pair_user_ids1, to_string(fromUid)));

                ASSERT_EQ(toUid,msg_GroupCreateRes.group().pair_uid());
            }

            list<string> from_user_group_ids;
            CModelGroup::getMemberGroupsByStatus(pCacheConn,from_user_group_ids,fromUid,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
            ASSERT_TRUE(list_string_contains(from_user_group_ids, to_string(groupInfoFromCache.group_id())));

            list<string> to_user_group_ids;
            CModelGroup::getMemberGroupsByStatus(pCacheConn,to_user_group_ids,fromUid,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
            ASSERT_TRUE(list_string_contains(to_user_group_ids, to_string(groupInfoFromCache.group_id())));

            list<string> member_ids;
            CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,group.group_id(),PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
            ASSERT_EQ(member_ids.size(),2);

            list<string> updated_member_ids;
            CModelGroup::getGroupMembersByUpdatedTime(pCacheConn,updated_member_ids,group.group_id(),0);
            ASSERT_EQ(updated_member_ids.size(),2);

            CRequest request_GroupCreateRes;
            request_GroupCreateRes.SetIsBusinessConn(false);
            request_GroupCreateRes.SetHandle(pMsgConn->GetHandle());
            request_GroupCreateRes.SetRequestPdu(request_next_GroupCreateReq.GetResponsePdu());
            ACTION_GROUP::GroupCreateResAction(&request_GroupCreateRes);
            PTP::Group::GroupCreateRes msg_final_GroupCreateRes;
            res = msg_final_GroupCreateRes.ParseFromArray(request_GroupCreateRes.GetResponsePdu()->GetBodyData(), (int)request_GroupCreateRes.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_GroupCreateRes.GetResponsePdu()->GetCommandId(),CID_GroupCreateRes);
            error = msg_final_GroupCreateRes.error();
            ASSERT_EQ(error,NO_ERROR);
        }
    }
    DEBUG_D("createGroup ========================================>>> end");

}
TEST(cachepool, create_group) {
    test_init();
    auto handle = (int)time(nullptr);
    CMsgSrvConn pMsgConn1;
    pMsgConn1.SetHandle(handle);
    login(&pMsgConn1,10011);
    uint32_t user_id_1 = pMsgConn1.GetUserId();

    CMsgSrvConn pMsgConn2;
    pMsgConn2.SetHandle(handle+1);
    login(&pMsgConn2,10012);
    uint32_t user_id_2 = pMsgConn2.GetUserId();

    CMsgSrvConn pMsgConn3;
    pMsgConn3.SetHandle(handle+2);
    login(&pMsgConn3,10013);
    uint32_t user_id_3 = pMsgConn3.GetUserId();

    auto groupType = PTP::Common::GROUP_TYPE_MULTI;
    createGroup1(&pMsgConn1,groupType,user_id_1,user_id_2,10011);

    groupType = PTP::Common::GROUP_TYPE_PAIR;
    createGroup1(&pMsgConn1,groupType,user_id_1,user_id_3,10011);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
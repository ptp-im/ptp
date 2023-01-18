#include <gtest/gtest.h>

#include "test_init.h"
#include "ptp_server/actions/GroupCreateAction.h"
#include "ptp_server/actions/models/ModelGroup.h"
#include "ptp_server/actions/GroupPreCreateAction.h"
#include "ptp_net/AccountManager.h"


using namespace PTP::Common;

TEST(test_Group, GroupCreateAction) {
    test_int();
    auto groupType = PTP::Common::GROUP_TYPE_MULTI;
    string captcha;
    uint32_t group_idx;
    uint32_t memberUid = 99999+1;

    PTP::Group::GroupPreCreateReq msg_GroupPreCreateReq;
    msg_GroupPreCreateReq.set_group_type(groupType);
    msg_GroupPreCreateReq.add_members(memberUid);

    uint16_t sep_no = 101;
    ImPdu pdu_GroupPreCreateReq;
    pdu_GroupPreCreateReq.SetPBMsg(&msg_GroupPreCreateReq,CID_GroupPreCreateReq,sep_no);
    CRequest request_GroupPreCreateReq;
    auto pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(time(nullptr));
    uint32_t uid = 99999+1;
    pMsgConn->SetUserId(uid);
    request_GroupPreCreateReq.SetHandle(pMsgConn->GetHandle());
    addMsgSrvConnByHandle(request_GroupPreCreateReq.GetHandle(),pMsgConn);
    request_GroupPreCreateReq.SetRequestPdu(&pdu_GroupPreCreateReq);
    ACTION_GROUP::GroupPreCreateReqAction(&request_GroupPreCreateReq);
    if(request_GroupPreCreateReq.GetResponsePdu()){
        ASSERT_EQ(request_GroupPreCreateReq.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request_GroupPreCreateReq.GetResponsePdu()->GetCommandId(),CID_GroupPreCreateReq);
        ASSERT_EQ(request_GroupPreCreateReq.IsNext(),true);

        CRequest request_next_GroupPreCreateReq;
        request_next_GroupPreCreateReq.SetIsBusinessConn(true);
        request_next_GroupPreCreateReq.SetRequestPdu(request_GroupPreCreateReq.GetResponsePdu());
        ACTION_GROUP::GroupPreCreateReqAction(&request_next_GroupPreCreateReq);

        PTP::Group::GroupPreCreateRes msg_GroupPreCreateRes;
        auto res = msg_GroupPreCreateRes.ParseFromArray(request_next_GroupPreCreateReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupPreCreateReq.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        ASSERT_EQ(request_next_GroupPreCreateReq.GetResponsePdu()->GetCommandId(),CID_GroupPreCreateRes);
        auto error = msg_GroupPreCreateRes.error();
        ASSERT_EQ(error,NO_ERROR);
        auto group_adr = msg_GroupPreCreateRes.group_adr();
        DEBUG_I("group_adr: %s",group_adr.c_str());
        auto group_idx1 = msg_GroupPreCreateRes.group_idx();
        DEBUG_I("group_idx: %d",group_idx1);
        auto captcha1 = msg_GroupPreCreateRes.captcha();
        DEBUG_I("captcha: %s",captcha1.c_str());
        auto auth_uid = msg_GroupPreCreateRes.auth_uid();
        DEBUG_I("auth_uid: %d",auth_uid);
        ASSERT_EQ(group_idx1> 0,true);
        ASSERT_EQ(captcha1,"");

        CRequest request_GroupPreCreateRes;
        request_GroupPreCreateRes.SetIsBusinessConn(false);
        request_GroupPreCreateRes.SetHandle(pMsgConn->GetHandle());
        request_GroupPreCreateRes.SetRequestPdu(request_next_GroupPreCreateReq.GetResponsePdu());
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
            DEBUG_I("captcha: %s",captcha1.c_str());
        }
    }
    if(group_idx > 0){
        PTP::Group::GroupCreateReq msg_GroupCreateReq;
        unsigned char sign65[65];
        AccountManager::getInstance(1001).signGroupMessage(captcha,(int32_t)group_idx,sign65);
        msg_GroupCreateReq.set_sign(sign65,65);
        msg_GroupCreateReq.set_group_idx(group_idx);
        msg_GroupCreateReq.set_captcha(captcha);
        msg_GroupCreateReq.set_group_type(groupType);
        msg_GroupCreateReq.set_name("name");
        msg_GroupCreateReq.set_avatar("avatar");
        msg_GroupCreateReq.add_members(memberUid+1);
        msg_GroupCreateReq.set_about("about");
        uint16_t sep_no = 101;
        ImPdu pdu_GroupCreateReq;
        pdu_GroupCreateReq.SetPBMsg(&msg_GroupCreateReq,CID_GroupCreateReq,sep_no);
        CRequest request_GroupCreateReq;
        auto pMsgConn = new CMsgSrvConn();
        pMsgConn->SetHandle(time(nullptr));
        pMsgConn->SetUserId(uid);
        request_GroupCreateReq.SetHandle(pMsgConn->GetHandle());
        addMsgSrvConnByHandle(request_GroupCreateReq.GetHandle(),pMsgConn);
        request_GroupCreateReq.SetRequestPdu(&pdu_GroupCreateReq);
        ACTION_GROUP::GroupCreateReqAction(&request_GroupCreateReq);
        if(request_GroupCreateReq.GetResponsePdu()){
            ASSERT_EQ(request_GroupCreateReq.GetResponsePdu()->GetSeqNum(),sep_no);
            ASSERT_EQ(request_GroupCreateReq.GetResponsePdu()->GetCommandId(),CID_GroupCreateReq);
            ASSERT_EQ(request_GroupCreateReq.IsNext(),true);

            CRequest request_next_GroupCreateReq;
            request_next_GroupCreateReq.SetIsBusinessConn(true);
            request_next_GroupCreateReq.SetRequestPdu(request_GroupCreateReq.GetResponsePdu());
            ACTION_GROUP::GroupCreateReqAction(&request_next_GroupCreateReq);

            PTP::Group::GroupCreateRes msg_GroupCreateRes;
            auto res = msg_GroupCreateRes.ParseFromArray(request_next_GroupCreateReq.GetResponsePdu()->GetBodyData(), (int)request_next_GroupCreateReq.GetResponsePdu()->GetBodyLength());
            ASSERT_EQ(res,true);
            ASSERT_EQ(request_next_GroupCreateReq.GetResponsePdu()->GetCommandId(),CID_GroupCreateRes);
            auto error = msg_GroupCreateRes.error();
            ASSERT_EQ(error,NO_ERROR);
            ASSERT_EQ(uid,msg_GroupCreateRes.auth_uid());
            DEBUG_I("auth_uid: %d",msg_GroupCreateRes.auth_uid());
            auto group = msg_GroupCreateRes.group();
            DEBUG_I("group_id: %d",group.group_id());
            DEBUG_I("group_idx: %d",group.group_idx());
            DEBUG_I("group_adr: %s",group.group_adr().c_str());
            auto group_members = msg_GroupCreateRes.group_members();
            DEBUG_I("group_members size: %d",group_members.size());

            for (const auto& member: group_members) {
                DEBUG_I("===>>member uid: %d",member.uid());
                DEBUG_I("member member_status: %d",member.member_status());
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
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

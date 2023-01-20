//
// Created by jack on 2023/1/15.
//
#include "test_init.h"
#include <gtest/gtest.h>

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


void test_init(){
    set_config_path(TEST_CONFIG_PATH);
    remove_config_path();

    replace_config("CacheInstances=auth,group",TEST_CONFIG_CacheInstances);
    replace_config("auth_host=127.0.0.1",TEST_CONFIG_auth_host);
    replace_config("auth_port=6379",TEST_CONFIG_auth_port);
    replace_config("auth_auth=",TEST_CONFIG_auth_auth);
    replace_config("auth_db=1",TEST_CONFIG_auth_db);
    replace_config("auth_maxconncnt=16",TEST_CONFIG_auth_maxconncnt);

    replace_config("group_host=127.0.0.1",TEST_CONFIG_group_host);
    replace_config("group_port=6379",TEST_CONFIG_group_port);
    replace_config("group_auth=",TEST_CONFIG_group_auth);
    replace_config("group_db=1",TEST_CONFIG_group_db);
    replace_config("group_maxconncnt=16",TEST_CONFIG_group_maxconncnt);

    remove_config_path();
    init_server_config();
    CacheManager::setConfigPath(get_config_path());
    CacheManager::getInstance();

}

CMsgSrvConn *test_init_msg_conn(){
    test_init();
    auto handle = (int)time(nullptr);
    auto *pMsgConn = new CMsgSrvConn();
    pMsgConn->SetHandle(handle);
    login(pMsgConn,10011);
    uint32_t user_id_1 = pMsgConn->GetUserId();

    CMsgSrvConn pMsgConn2;
    pMsgConn2.SetHandle(handle+1);
    login(&pMsgConn2,10012);
    uint32_t user_id_2 = pMsgConn2.GetUserId();
    CMsgSrvConn pMsgConn3;
    pMsgConn3.SetHandle(handle+2);
    login(&pMsgConn3,10013);
    uint32_t user_id_3 = pMsgConn3.GetUserId();

    auto groupType = PTP::Common::GROUP_TYPE_MULTI;
    createGroup(pMsgConn,groupType,user_id_1,user_id_2);
    CImUserManager::GetInstance()->RemoveImUserById(pMsgConn2.GetUserId());
    CImUserManager::GetInstance()->RemoveImUserById(pMsgConn3.GetUserId());
    groupType = PTP::Common::GROUP_TYPE_PAIR;
    createGroup(pMsgConn,groupType,user_id_1,user_id_3);
    return pMsgConn;
}

void login(CMsgSrvConn *pMsgConn,uint32_t accountId){
    DEBUG_D("login ========================================>>>");
    PTP::Auth::AuthCaptchaReq msg;
    uint16_t sep_no = time(nullptr);
    ImPdu pdu;
    pdu.SetPBMsg(&msg,CID_AuthCaptchaReq,sep_no);
    CRequest request;
    request.SetHandle(pMsgConn->GetHandle());
    pMsgConn->SetHandle(request.GetHandle());
    addMsgSrvConnByHandle(request.GetHandle(),pMsgConn);
    request.SetRequestPdu(&pdu);

    ACTION_AUTH::AuthCaptchaReqAction(&request);
    if(request.GetResponsePdu()){
        ASSERT_EQ(request.GetResponsePdu()->GetSeqNum(),sep_no);
        ASSERT_EQ(request.GetResponsePdu()->GetCommandId(),CID_AuthCaptchaRes);
        PTP::Auth::AuthCaptchaRes msg_rsp;
        auto res = msg_rsp.ParseFromArray(request.GetResponsePdu()->GetBodyData(), (int)request.GetResponsePdu()->GetBodyLength());
        ASSERT_EQ(res,true);
        auto error = msg_rsp.error();
        ASSERT_EQ(error,NO_ERROR);
        auto captcha = msg_rsp.captcha();
        ASSERT_EQ(captcha.size(),6);
        DEBUG_I("captcha: %s",captcha.c_str());
        auto server_address = bytes_to_hex_string((unsigned char *)msg_rsp.address().data(),20);
        ASSERT_EQ(msg_rsp.address().size(),20);
        DEBUG_I("server_address: %s", server_address.c_str());
        auto iv = msg_rsp.iv();
        ASSERT_EQ(iv.size(),16);
        DEBUG_I("iv: %s", bytes_to_hex_string((unsigned char *)iv.data(), 16).c_str());
        auto aad = msg_rsp.aad();
        ASSERT_EQ(aad.size(),16);
        DEBUG_I("add: %s", bytes_to_hex_string((unsigned char *)aad.data(), 16).c_str());
        auto sign = msg_rsp.sign();
        ASSERT_EQ(sign.size(),65);
        DEBUG_I("sign: %s", bytes_to_hex_string((unsigned char *)sign.data(), 65).c_str());

        PTP::Auth::AuthLoginReq msg_login;

        unsigned char sign65[65];
        string client_address = AccountManager::getInstance(accountId).getAccountAddress();
        AccountManager::getInstance(accountId).signMessage(captcha,sign65);
        DEBUG_I("client_address: %s", client_address.c_str());
        DEBUG_I("client sign: %s", bytes_to_hex_string(sign65,65).c_str());

        string msg_data = format_sign_msg_data(captcha,SignMsgType_ptp);
        DEBUG_D("sign msg_data:%s", msg_data.c_str());
        unsigned char pub_key_33[33];
        string address_hex;
        bool ret = recover_pub_key_and_address_from_sig(sign65, msg_data, pub_key_33, address_hex);
        DEBUG_D("client pub_key_33 rec:%s", bytes_to_hex_string(pub_key_33, 33).c_str());
        DEBUG_D("client address rec:%s", address_hex.c_str());

        msg_login.set_address(client_address);
        msg_login.set_captcha(captcha);
        msg_login.set_sign(sign65,65);
        msg_login.set_client_type(PTP::Common::CLIENT_TYPE_ANDROID);
        msg_login.set_client_version("android/1.1");
        ImPdu pdu_login;
        sep_no+=1;
        pdu_login.SetPBMsg(&msg_login,CID_AuthLoginReq,sep_no);
        CRequest request_login;
        request_login.SetHandle(request.GetHandle());
        request_login.SetRequestPdu(&pdu_login);
        ACTION_AUTH::AuthLoginReqAction(&request_login);
        if(request_login.HasNext()){
            ASSERT_EQ(request_login.GetNextResponsePdu()->GetSeqNum(),sep_no);
            ASSERT_EQ(request_login.GetNextResponsePdu()->GetCommandId(),CID_ServerLoginReq);
            PTP::Server::ServerLoginReq msg_login_rsp;
            res = msg_login_rsp.ParseFromArray(request_login.GetNextResponsePdu()->GetBodyData(), (int)request_login.GetNextResponsePdu()->GetBodyLength());

            ASSERT_EQ(pMsgConn->GetAddress(),client_address);
            ASSERT_EQ(pMsgConn->GetUserId()>0,false);
            ASSERT_EQ(res,true);
            ASSERT_EQ(msg_login_rsp.captcha(),msg_login.captcha());
            ASSERT_EQ(
                    msg_login_rsp.address(),
                    client_address);

            ASSERT_EQ(
                    bytes_to_hex_string((unsigned char *)msg_login_rsp.sign().data(),65),
                    bytes_to_hex_string((unsigned char *)msg_login.sign().data(),65));

            ImPdu pdu_1;
            pdu_1.SetPBMsg(&msg_login_rsp,CID_ServerLoginReq,sep_no);
            CRequest request_1;
            request_1.SetIsBusinessConn(true);
            request_1.SetRequestPdu(&pdu_1);
            ACTION_SERVER::ServerLoginReqAction(&request_1);
            if(request_1.HasNext()){
                auto nextPdu = request_1.GetNextResponsePdu();
                ASSERT_EQ(nextPdu->GetSeqNum(),sep_no);
                ASSERT_EQ(nextPdu->GetCommandId(),CID_ServerLoginRes);
                PTP::Server::ServerLoginRes msg_login_res_rsp;
                res = msg_login_res_rsp.ParseFromArray(nextPdu->GetBodyData(), (int)nextPdu->GetBodyLength());
                ASSERT_EQ(res,true);
                error = msg_login_res_rsp.error();
                ASSERT_EQ(error,NO_ERROR);
                ASSERT_EQ(true,msg_login_res_rsp.user_info().uid() > 0);
                DEBUG_I("address: %s",msg_login_res_rsp.user_info().address().c_str());
                ASSERT_EQ(
                        msg_login_res_rsp.user_info().address(),
                        client_address);

                ImPdu pdu_2;
                pdu_2.SetPBMsg(&msg_login_res_rsp,CID_ServerLoginRes,sep_no);
                CRequest request_2;
                request_2.SetHandle(request.GetHandle());
                request_2.SetIsBusinessConn(false);
                request_2.SetRequestPdu(&pdu_2);

                ACTION_SERVER::ServerLoginResAction(&request_2);
                if(request_2.HasNext()){
                    auto nextPdu2 = request_2.GetNextResponsePdu();
                    ASSERT_EQ(nextPdu2->GetSeqNum(),sep_no);
                    PTP::Auth::AuthLoginRes msg_login_res_rsp2;
                    res = msg_login_res_rsp2.ParseFromArray(nextPdu2->GetBodyData(), (int)nextPdu2->GetBodyLength());
                    ASSERT_EQ(res,true);
                    error = msg_login_res_rsp2.error();
                    ASSERT_EQ(error,NO_ERROR);
                    ASSERT_EQ(true,msg_login_res_rsp2.user_info().uid() > 0);
                    ASSERT_EQ(nextPdu2->GetCommandId(),CID_AuthLoginRes);
                    ASSERT_EQ(pMsgConn->GetAddress(),client_address);
                    ASSERT_EQ(pMsgConn->GetUserId(),msg_login_res_rsp2.user_info().uid());
                    ASSERT_EQ(pMsgConn->GetUserId(),msg_login_res_rsp2.user_info().uid());

                    DEBUG_D("uid:%d",msg_login_res_rsp2.user_info().uid());
                    DEBUG_D("pub_key:%s", bytes_to_hex_string((unsigned char *)msg_login_res_rsp2.user_info().pub_key().data(),33).c_str());
                    DEBUG_D("address:%s",msg_login_res_rsp2.user_info().address().c_str());
                    DEBUG_D("avatar:%s",msg_login_res_rsp2.user_info().avatar().c_str());
                    DEBUG_D("user_name:%s",msg_login_res_rsp2.user_info().user_name().c_str());
                    DEBUG_D("first_name:%s",msg_login_res_rsp2.user_info().first_name().c_str());
                    DEBUG_D("last_name:%s",msg_login_res_rsp2.user_info().last_name().c_str());
                    DEBUG_D("sign_info:%s",msg_login_res_rsp2.user_info().sign_info().c_str());
                    DEBUG_D("login_time:%d",msg_login_res_rsp2.user_info().login_time());
                    DEBUG_D("status:%d",msg_login_res_rsp2.user_info().status());

                    CacheManager *pCacheManager = CacheManager::getInstance();
                    CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
                    UserInfo userInfo;
                    CModelBuddy::getUserInfo(pCacheConn,&userInfo,pMsgConn->GetUserId());
                    ASSERT_EQ(userInfo.uid(),msg_login_res_rsp2.user_info().uid());
                    DEBUG_D("login_time:%d",userInfo.login_time());
                }

            }
        }
    }

    DEBUG_D("login ========================================>>> end");
}

void createGroup(CMsgSrvConn *pMsgConn,PTP::Common::GroupType groupType,uint32_t fromUid,uint32_t toUid){
    DEBUG_D("createGroup ========================================>>>");
    string captcha;
    uint32_t group_idx = 0;

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
                DEBUG_I("captcha: %s",captcha1.c_str());
            }
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
                DEBUG_I("E_GROUP_CREATE_PAIR_EXISTS");
                return;
            }
            ASSERT_EQ(error,NO_ERROR);
            ASSERT_EQ(fromUid,msg_GroupCreateRes.auth_uid());
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
            CModelGroup::getMemberGroups(pCacheConn,from_user_group_ids,fromUid);
            ASSERT_TRUE(list_string_contains(from_user_group_ids, to_string(groupInfoFromCache.group_id())));

            list<string> to_user_group_ids;
            CModelGroup::getMemberGroups(pCacheConn,to_user_group_ids,fromUid);
            ASSERT_TRUE(list_string_contains(to_user_group_ids, to_string(groupInfoFromCache.group_id())));

            list<string> member_ids;
            CModelGroup::getGroupMembersStatus(pCacheConn,member_ids,group.group_id(),PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
            ASSERT_EQ(member_ids.size(),2);

            list<string> updated_member_ids;
            CModelGroup::getGroupMembersUpdated(pCacheConn,updated_member_ids,group.group_id(),0);
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
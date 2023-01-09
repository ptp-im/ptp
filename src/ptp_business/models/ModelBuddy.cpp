/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelBuddy.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelBuddy.h"
#include "CachePool.h"

CModelBuddy* CModelBuddy::m_pInstance = NULL;

CModelBuddy::CModelBuddy()
{
    
}

CModelBuddy::~CModelBuddy()
{
    
}

CModelBuddy* CModelBuddy::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelBuddy();
    }
    return m_pInstance;
}

void  CModelBuddy::BuddyGetALLReq(PTP::Buddy::BuddyGetALLReq* msg, PTP::Buddy::BuddyGetALLRes* msg_rsp, PTP::Common::ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        list<string> argv;
        list<string> ret_list;
        argv.emplace_back("ZRANGE");
        argv.push_back(MEMBER_PAIR_UPDATE_PREFIX + to_string(msg->auth_uid()));
        argv.push_back(to_string(msg->buddy_updated_time()));
        argv.emplace_back("+INF");
        argv.emplace_back("BYSCORE");
        pCacheConn->exec(&argv,&ret_list);

        list<string> argv_users;
        list<string> list_users;
        list<uint32_t> pairs;
        argv_users.emplace_back("MGET");
        for(auto it = ret_list.begin();it != ret_list.end();it++){
            string pair_uid = it->c_str();
            if(string2int(pair_uid) == msg->auth_uid()){
                continue;
            }
            pairs.push_back(string2int(pair_uid));
            argv_users.push_back(CACHE_USER_ID_ADR_PREFIX + pair_uid);
            argv_users.push_back(CACHE_USER_ID_AVATAR_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_USERNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_PUB_KEY_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_STATUS_PREFIX+ pair_uid);
        }
        pCacheConn->exec(&argv_users,&list_users);

        auto it_pair = pairs.begin();
        for (auto it = list_users.begin(); it != list_users.end() ; it++) {
            PTP::Common::UserInfo * buddy = msg_rsp->add_buddy_list();
            buddy->set_uid(*it_pair);
            buddy->set_address(it->c_str());
            advance(it,1);
            buddy->set_avatar(it->c_str());
            advance(it,1);
            buddy->set_nick_name(it->c_str());
            advance(it,1);
            buddy->set_user_name(it->c_str());
            advance(it,1);
            buddy->set_pub_key(it->c_str());
            advance(it,1);
            buddy->set_sign_info(it->c_str());
            advance(it,1);
            buddy->set_first_name(it->c_str());
            advance(it,1);
            buddy->set_last_name(it->c_str());
            advance(it,1);
            buddy->set_login_time(string2int(it->c_str()));
            advance(it,1);
            buddy->set_status(string2int(*it));
            it_pair++;
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void CModelBuddy::BuddyQueryListReq(PTP::Buddy::BuddyQueryListReq* msg, PTP::Buddy::BuddyQueryListRes* msg_rsp, ERR& error){

    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        list<uint32_t> user_ids;

        auto addressList = msg->params().addresslist();
        auto usernameList = msg->params().usernamelist();

        list<string> argv_users1;
        list<string> list_user_ids;
        if(addressList.size() > 0){
            argv_users1.emplace_back("MGET");
            for(auto it = addressList.begin();it != addressList.end();it++){
                string address = it->c_str();
                argv_users1.push_back(CACHE_USER_ADR_ID_PREFIX + address);
            }
            pCacheConn->exec(&argv_users1,&list_user_ids);
        }

        if(usernameList.size() > 0){
            argv_users1.emplace_back("MGET");
            for(auto it = usernameList.begin();it != usernameList.end();it++){
                string username = it->c_str();
                argv_users1.push_back(CACHE_USER_USERNAME_UID_PREFIX + username);
            }
            pCacheConn->exec(&argv_users1,&list_user_ids);
        }
        if(!list_user_ids.empty()){
            for(auto it = list_user_ids.begin();it != list_user_ids.end();it++){
                string user_id_str = it->c_str();
                user_ids.push_back(string2int(user_id_str));
            }
        }
        list<string> argv_users;
        list<string> list_users;
        argv_users.emplace_back("MGET");
        for(unsigned int & user_id : user_ids){
            string pair_uid = int2string(user_id);
            argv_users.push_back(CACHE_USER_ID_ADR_PREFIX + pair_uid);
            argv_users.push_back(CACHE_USER_ID_AVATAR_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_USERNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_PUB_KEY_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_STATUS_PREFIX+ pair_uid);
        }

        pCacheConn->exec(&argv_users,&list_users);
        if(!list_users.empty()){
            auto it_user = user_ids.begin();
            for (auto it = list_users.begin(); it != list_users.end() ; it++) {
                PTP::Common::UserInfo * buddy = msg_rsp->add_buddy_list();
                buddy->set_uid(*it_user);
                buddy->set_address(it->c_str());
                advance(it,1);
                buddy->set_avatar(it->c_str());
                advance(it,1);
                buddy->set_nick_name(it->c_str());
                advance(it,1);
                buddy->set_user_name(it->c_str());
                advance(it,1);
                buddy->set_pub_key(it->c_str());
                advance(it,1);
                buddy->set_sign_info(it->c_str());
                advance(it,1);
                buddy->set_first_name(it->c_str());
                advance(it,1);
                buddy->set_last_name(it->c_str());
                advance(it,1);
                buddy->set_login_time(string2int(it->c_str()));
                advance(it,1);
                buddy->set_status(string2int(*it));
                it_user++;
            }
        }

        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void  CModelBuddy::BuddyGetListReq(PTP::Buddy::BuddyGetListReq* msg, PTP::Buddy::BuddyGetListRes* msg_rsp, PTP::Common::ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        auto user_ids = msg->user_ids();

        list<string> argv_users;
        list<string> list_users;
        argv_users.emplace_back("MGET");
        for(auto it = user_ids.begin();it != user_ids.end();it++){
            string pair_uid = int2string(*it);
            argv_users.push_back(CACHE_USER_ID_ADR_PREFIX + pair_uid);
            argv_users.push_back(CACHE_USER_ID_AVATAR_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_USERNAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_PUB_KEY_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX+ pair_uid);
            argv_users.push_back(CACHE_USER_ID_STATUS_PREFIX+ pair_uid);
        }
        pCacheConn->exec(&argv_users,&list_users);

        auto it_user = user_ids.begin();

        for (auto it = list_users.begin(); it != list_users.end() ; it++) {
            PTP::Common::UserInfo * buddy = msg_rsp->add_buddy_list();
            buddy->set_uid(*it_user);
            buddy->set_address(it->c_str());
            advance(it,1);
            buddy->set_avatar(it->c_str());
            advance(it,1);
            buddy->set_nick_name(it->c_str());
            advance(it,1);
            buddy->set_user_name(it->c_str());
            advance(it,1);
            buddy->set_pub_key(it->c_str());
            advance(it,1);
            buddy->set_sign_info(it->c_str());
            advance(it,1);
            buddy->set_first_name(it->c_str());
            advance(it,1);
            buddy->set_last_name(it->c_str());
            advance(it,1);
            buddy->set_login_time(string2int(it->c_str()));
            advance(it,1);
            buddy->set_status(string2int(*it));
            it_user++;
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelBuddy::BuddyGetStatReq(PTP::Buddy::BuddyGetStatReq* msg, PTP::Buddy::BuddyGetStatRes* msg_rsp, ERR& error){

}

void  CModelBuddy::BuddyModifyReq(PTP::Buddy::BuddyModifyReq* msg, PTP::Buddy::BuddyModifyRes* msg_rsp, PTP::Common::ERR& error)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }

        auto action = msg->buddy_modify_action();
        list<string> argv;
        msg_rsp->set_value(msg->value());
        msg_rsp->set_buddy_modify_action(action);
        switch (action) {
            case PTP::Common::BuddyModifyAction_avatar:
                argv.push_back(CACHE_USER_ID_AVATAR_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                break;
            case PTP::Common::BuddyModifyAction_nickname:
                argv.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                break;
            case PTP::Common::BuddyModifyAction_user_name:
            {
                auto uid = pCacheConn->get(CACHE_USER_USERNAME_UID_PREFIX+msg->value());
                if(!uid.empty()){
                    error = PTP::Common:: E_USERNAME_EXISTS;
                    break;
                }
                argv.push_back(CACHE_USER_ID_USERNAME_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                argv.push_back(CACHE_USER_USERNAME_UID_PREFIX+ msg->value());
                argv.push_back(to_string(msg->auth_uid()));
                break;
            }
            case PTP::Common::BuddyModifyAction_sign_info:
                argv.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                break;
            case PTP::Common::BuddyModifyAction_first_name:
                argv.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                break;
            case PTP::Common::BuddyModifyAction_last_name:
                argv.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ to_string(msg->auth_uid()));
                argv.push_back(msg->value());
                break;
            default:
                break;
        }
        if(!argv.empty()){
            argv.push_front("MSET");
            pCacheConn->exec(&argv, nullptr);

            list<string> argv1;
            list<string> ret_list;
            argv1.emplace_back("ZRANGE");
            argv1.push_back(MEMBER_PAIR_UPDATE_PREFIX + to_string(msg->auth_uid()));
            argv1.emplace_back("0");
            argv1.emplace_back("-1");
            pCacheConn->exec(&argv1, &ret_list);
            if(!ret_list.empty()){
                for (auto it = ret_list.begin();it!= ret_list.end();it++) {
                    string pair_uid = it->c_str();
                    msg_rsp->add_notify_pairs(string2int(pair_uid));
                }
            }
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void  CModelBuddy::BuddyModifyUpdatePair(PTP::Buddy::BuddyModifyUpdatePair* msg)
{
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("auth");
    while (true) {
        if (!pCacheConn) {
            log_error("error pCacheConn");
            break;
        }
        auto pair_uid_list = msg->pair_uid_list();
        auto updated_time = time(nullptr);
        for(auto it = pair_uid_list.begin();it != pair_uid_list.end();it++){
            list<string> argv2;
            uint32_t pair_uid = *it;
            argv2.push_back("ZADD");
            argv2.push_back(MEMBER_PAIR_UPDATE_PREFIX + to_string(pair_uid));
            argv2.push_back(to_string(updated_time));
            argv2.push_back(to_string(msg->auth_uid()));
            pCacheConn->exec(&argv2, nullptr);
            log("update pair members %d -> %d",pair_uid,msg->auth_uid());
        }

        list<string> argv2;
        list<string> ret_list2;

        argv2.emplace_back("ZRANGE");
        argv2.push_back(MEMBER_GROUP_UPDATE_KEY_PREFIX + to_string(msg->auth_uid()));
        argv2.emplace_back("0");
        argv2.emplace_back("-1");
        pCacheConn->exec(&argv2, &ret_list2);
        for(auto it = ret_list2.begin();it != ret_list2.end();it++){
            list<string> argv3;
            string group_id = it->c_str();
            argv3.push_back("ZADD");
            argv3.push_back(GROUP_MEMBER_UPDATE_KEY_PREFIX + group_id);
            argv3.push_back(to_string(updated_time));
            argv3.push_back(to_string(msg->auth_uid()));
            pCacheConn->exec(&argv3, nullptr);
            log("update groups %s -> %d",group_id.c_str(),msg->auth_uid());
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

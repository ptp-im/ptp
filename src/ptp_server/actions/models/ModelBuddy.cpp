/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelBuddy.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
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

void CModelBuddy::handleUserInfoCacheArgv(list<string> &argv,const string& uid)
{
    argv.push_back(CACHE_USER_ID_ADR_PREFIX + uid);
    argv.push_back(CACHE_USER_ID_AVATAR_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_USERNAME_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_PUB_KEY_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_LOGIN_TIME_PREFIX+ uid);
    argv.push_back(CACHE_USER_ID_STATUS_PREFIX+ uid);
}

void CModelBuddy::handleUserInfoCache(PTP::Common::UserInfo *user,list<string>::iterator it){
    user->set_address(it->c_str());
    advance(it,1);
    user->set_avatar(it->c_str());
    advance(it,1);
    user->set_nick_name(it->c_str());
    advance(it,1);
    user->set_user_name(it->c_str());
    advance(it,1);
    user->set_pub_key(hex_to_string(it->substr(2)));
    advance(it,1);
    user->set_sign_info(it->c_str());
    advance(it,1);
    user->set_first_name(it->c_str());
    advance(it,1);
    user->set_last_name(it->c_str());
    advance(it,1);
    user->set_login_time(string2int(*it));
    advance(it,1);
    user->set_status(it->empty() ? (uint32_t)0 : string2int(*it));
}

void CModelBuddy::getUserInfo(CacheConn *pCacheConn,PTP::Common::UserInfo *user,uint32_t user_id){
    list<string> argv;
    list<string> list_ret;
    argv.emplace_back("MGET");
    CModelBuddy::handleUserInfoCacheArgv(argv,int2string(user_id));
    pCacheConn->exec(&argv, &list_ret);
    auto it = list_ret.begin();
    CModelBuddy::handleUserInfoCache(user,it);
    user->set_uid(user_id);
}

void CModelBuddy::getUserIdListByAddress(CacheConn *pCacheConn,list<string> &addressList,list<string> &user_ids){
    if(!addressList.empty()){
        list<string> argv;
        argv.emplace_back("MGET");
        for(const string& address:addressList){
            argv.push_back(CACHE_USER_ADR_ID_PREFIX + address);
        }
        pCacheConn->exec(&argv, &user_ids);
    }
}

void CModelBuddy::getUserIdListByUserName(CacheConn *pCacheConn,list<string> &usernameList,list<string> &user_ids){
    if(!usernameList.empty()){
        list<string> argv;
        argv.emplace_back("MGET");
        for(const string& username:usernameList){
            argv.push_back(CACHE_USER_USERNAME_UID_PREFIX + username);
        }
        pCacheConn->exec(&argv, &user_ids);
    }
}

void CModelBuddy::modify(CacheConn *pCacheConn,BuddyModifyAction action,const string &value,uint32_t user_id,ERR &error){
    list<string> argv;
    string user_id_str = to_string(user_id);
    switch (action) {
        case PTP::Common::BuddyModifyAction_avatar:
            argv.push_back(CACHE_USER_ID_AVATAR_PREFIX+ user_id_str);
            argv.push_back(value);
            break;
        case PTP::Common::BuddyModifyAction_nickname:
            argv.push_back(CACHE_USER_ID_NICKNAME_PREFIX+ user_id_str);
            argv.push_back(value);
            break;
        case PTP::Common::BuddyModifyAction_user_name:
        {
            auto uid = pCacheConn->get(CACHE_USER_USERNAME_UID_PREFIX+value);
            if(!uid.empty()){
                error = PTP::Common::E_USERNAME_EXISTS;
                break;
            }
            argv.push_back(CACHE_USER_ID_USERNAME_PREFIX+ user_id_str);
            argv.push_back(value);
            argv.push_back(CACHE_USER_USERNAME_UID_PREFIX+ value);
            argv.push_back(user_id_str);
            break;
        }
        case PTP::Common::BuddyModifyAction_sign_info:
            argv.push_back(CACHE_USER_ID_SIGN_INFO_PREFIX+ user_id_str);
            argv.push_back(value);
            break;
        case PTP::Common::BuddyModifyAction_first_name:
            argv.push_back(CACHE_USER_ID_FIRST_NAME_PREFIX+ user_id_str);
            argv.push_back(value);
            break;
        case PTP::Common::BuddyModifyAction_last_name:
            argv.push_back(CACHE_USER_ID_LAST_NAME_PREFIX+ user_id_str);
            argv.push_back(value);
            break;
        default:
            error = PTP::Common::E_BUDDY_MODIFY_INVALID_ACTION;
            break;
    }
    if(!argv.empty()){
        argv.push_front("MSET");
        pCacheConn->exec(&argv, nullptr);
    }
}

CModelBuddy* CModelBuddy::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelBuddy();
    }
    return m_pInstance;
}

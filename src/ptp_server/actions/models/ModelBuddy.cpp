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
    user->set_uid(string2int(*it));
    advance(it,1);
    user->set_avatar(it->c_str());
    advance(it,1);
    user->set_nick_name(it->c_str());
    advance(it,1);
    user->set_user_name(it->c_str());
    advance(it,1);
    user->set_pub_key(it->c_str());
    advance(it,1);
    user->set_sign_info(it->c_str());
    advance(it,1);
    user->set_first_name(it->c_str());
    advance(it,1);
    user->set_last_name(it->c_str());
    advance(it,1);
    user->set_login_time(string2int(it->c_str()));
    advance(it,1);
    user->set_status(string2int(*it));

}


CModelBuddy* CModelBuddy::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelBuddy();
    }
    return m_pInstance;
}

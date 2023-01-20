/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelBuddy.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_BUDDY_H_
#define MODEL_BUDDY_H_

#include <list>
#include "../../CachePool.h"
#include "PTP.Buddy.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelBuddy {
public:
virtual ~CModelBuddy();
    static CModelBuddy* getInstance();
    static void modify(CacheConn *pCacheConn,BuddyModifyAction action,const string &value,uint32_t user_id,ERR &error);
    static void handleUserInfoCacheArgv(list<string> &argv,const string& uid);
    static void handleUserInfoCache(PTP::Common::UserInfo * user,list<string>::iterator it);
    static void getUserInfo(CacheConn *pCacheConn,PTP::Common::UserInfo *user,uint32_t user_id);
    static void getUserIdListByAddress(CacheConn *pCacheConn,list<string> &addressList,list<string> &user_ids);
    static void getUserIdListByUserName(CacheConn *pCacheConn,list<string> &usernameList,list<string> &user_ids);
private:
    CModelBuddy();
private:
    static CModelBuddy*    m_pInstance;
};

#endif /* MODEL_BUDDY_H_ */

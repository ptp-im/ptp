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
#include "PTP.Buddy.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelBuddy {
public:
virtual ~CModelBuddy();
    static CModelBuddy* getInstance();
    static void handleUserInfoCacheArgv(list<string> &argv,const string& uid);
    static void handleUserInfoCache(PTP::Common::UserInfo * user,list<string>::iterator it);
private:
    CModelBuddy();
private:
    static CModelBuddy*    m_pInstance;
};

#endif /* MODEL_BUDDY_H_ */

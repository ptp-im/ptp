/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelBuddy.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Tuesday, November 29, 2022
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_BUDDY_H_
#define MODEL_BUDDY_H_

#include "PTP.Buddy.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelBuddy {
public:
virtual ~CModelBuddy();

    static CModelBuddy* getInstance();
    void BuddyGetALLReq(PTP::Buddy::BuddyGetALLReq* msg, PTP::Buddy::BuddyGetALLRes* msg_rsp, ERR& error);
    void BuddyGetListReq(PTP::Buddy::BuddyGetListReq* msg, PTP::Buddy::BuddyGetListRes* msg_rsp, ERR& error);
    void BuddyGetStatReq(PTP::Buddy::BuddyGetStatReq* msg, PTP::Buddy::BuddyGetStatRes* msg_rsp, ERR& error);
    void BuddyModifyReq(PTP::Buddy::BuddyModifyReq* msg, PTP::Buddy::BuddyModifyRes* msg_rsp, ERR& error);
    void BuddyQueryListReq(PTP::Buddy::BuddyQueryListReq* msg, PTP::Buddy::BuddyQueryListRes* msg_rsp, ERR& error);
    void BuddyModifyUpdatePair(PTP::Buddy::BuddyModifyUpdatePair* msg);
private:
    CModelBuddy();
    
private:
    static CModelBuddy*    m_pInstance;
};

#endif /* MODEL_BUDDY_H_ */

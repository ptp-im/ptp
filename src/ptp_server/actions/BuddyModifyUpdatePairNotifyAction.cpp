/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyUpdatePairNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Thursday, January 19, 2023
 *   desc：
 *
================================================================*/
#include "BuddyModifyUpdatePairNotifyAction.h"
#include "PTP.Buddy.pb.h"
#include "models/ModelBuddy.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyModifyUpdatePairNotifyAction(CRequest* request){
        PTP::Buddy::BuddyModifyUpdatePairNotify msg;
        CacheManager *pCacheManager = CacheManager::getInstance();
        CacheConn *pCacheConnGroup = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            if(!pCacheConnGroup){
                break;
            }
            auto auth_uid = msg.auth_uid();
            uint32_t created_time = time(nullptr);
            for(uint32_t user_id:msg.pair_uid_list()){
                CModelGroup::updateGroupMemberPairUpdate(pCacheConnGroup,user_id,auth_uid,created_time);
            }
            break;
        }
        if(pCacheConnGroup){
            pCacheManager->RelCacheConn(pCacheConnGroup);
        }
    }
};


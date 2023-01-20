/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetALLAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyGetALLAction.h"
#include "PTP.Buddy.pb.h"
#include "ImUser.h"
#include "models/ModelBuddy.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyGetALLReqAction(CRequest* request){
        PTP::Buddy::BuddyGetALLReq msg; 
        PTP::Buddy::BuddyGetALLRes msg_rsp;
        ERR error = NO_ERROR;
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            if(!request->IsBusinessConn()){
              auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
              if(!pMsgConn){
                  DEBUG_E("not found pMsgConn");
                  return;
              }
             msg.set_auth_uid(pMsgConn->GetUserId());
             request->Next(&msg,CID_BuddyGetALLReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
                CacheConn *pCacheConnGroup = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);

                while (true) {
                    if (!pCacheConn || !pCacheConnGroup) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    list<string> pair_user_ids;
                    CModelGroup::getUserGroupPairs(pCacheConnGroup,pair_user_ids,auth_uid,msg.buddy_updated_time());

                    for(const auto& it : pair_user_ids){
                        uint32_t pair_uid = string2int(it);
                        if(pair_uid == auth_uid){
                            continue;
                        }
                        PTP::Common::UserInfo *buddy = msg_rsp.add_buddy_list();
                        CModelBuddy::getUserInfo(pCacheConn,buddy,pair_uid);
                        CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddress(buddy->address());
                        buddy->set_is_online(pImUser != nullptr);
                    }

                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    msg_rsp.set_buddy_updated_time(time(nullptr));
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                if (pCacheConnGroup) {
                    pCacheManager->RelCacheConn(pCacheConnGroup);
                }
                request->SendResponseMsg(&msg_rsp,CID_BuddyGetALLRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_BuddyGetALLRes,request->GetSeqNum());
        }
    }
    void BuddyGetALLResAction(CRequest* request){
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


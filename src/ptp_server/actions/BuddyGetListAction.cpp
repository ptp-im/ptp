/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetListAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyGetListAction.h"
#include "PTP.Buddy.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelBuddy.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyGetListReqAction(CRequest* request){
        PTP::Buddy::BuddyGetListReq msg; 
        PTP::Buddy::BuddyGetListRes msg_rsp;
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
             request->Next(&msg,CID_BuddyGetListReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    auto user_ids = msg.user_ids();

                    list<string> argv_users;
                    list<string> list_users;
                    argv_users.emplace_back("MGET");
                    for(unsigned int & user_id : user_ids){
                        string pair_uid = int2string(user_id);
                        CModelBuddy::handleUserInfoCacheArgv(argv_users,pair_uid);
                    }
                    pCacheConn->exec(&argv_users,&list_users);

                    auto it_user = user_ids.begin();

                    for (auto it = list_users.begin(); it != list_users.end() ; it++) {
                        PTP::Common::UserInfo * buddy = msg_rsp.add_buddy_list();
                        buddy->set_uid(*it_user);
                        CModelBuddy::handleUserInfoCache(buddy,it);
                        it_user++;
                    }

                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_BuddyGetListRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_BuddyGetListRes,request->GetSeqNum());
        }
    }
    void BuddyGetListResAction(CRequest* request){
        // PTP::Buddy::BuddyGetListRes msg;
        // auto error = msg.error();
        // while (true){
        //     if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
        //     {
        //         error = E_PB_PARSE_ERROR;
        //         break;
        //     }
        //     if(!request->IsBusinessConn()){
        //       uint32_t handle = request->GetHandle();
        //       auto pMsgConn = FindMsgSrvConnByHandle(handle);
        //       if(!pMsgConn){
        //           DEBUG_E("not found pMsgConn");
        //           return;
        //       }
        //       if(error != PTP::Common::NO_ERROR){
        //           break;
        //       }
        //     }else{
        //       if(error != PTP::Common::NO_ERROR){
        //           break;
        //       }
        //     }
        //     break;
        // }
        // msg.set_error(error);
        // request->SendResponseMsg(&msg,CID_BuddyGetListRes,request->GetSeqNum());
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


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
#include "MsgSrvConn.h"
#include "models/ModelBuddy.h"

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

                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    list<string> argv;
                    list<string> ret_list;
                    argv.emplace_back("ZRANGE");
                    argv.push_back(MEMBER_PAIR_UPDATE_PREFIX + to_string(auth_uid));
                    argv.push_back(to_string(msg.buddy_updated_time()));
                    argv.emplace_back("+INF");
                    argv.emplace_back("BYSCORE");
                    pCacheConn->exec(&argv,&ret_list);

                    list<string> argv_users;
                    list<string> list_users;
                    list<uint32_t> pairs;
                    argv_users.emplace_back("MGET");

                    for(auto it = ret_list.begin();it != ret_list.end();it++){
                        string pair_uid = *it;
                        if(string2int(pair_uid) == auth_uid){
                            continue;
                        }
                        pairs.push_back(string2int(pair_uid));
                        CModelBuddy::handleUserInfoCacheArgv(argv_users,pair_uid);
                    }
                    pCacheConn->exec(&argv_users,&list_users);
                    auto it_pair = pairs.begin();
                    for (auto it = list_users.begin(); it != list_users.end() ; it++) {
                        PTP::Common::UserInfo *buddy = msg_rsp.add_buddy_list();
                        CModelBuddy::handleUserInfoCache(buddy,it);
                        it_pair++;
                    }
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    msg_rsp.set_buddy_updated_time(time(nullptr));
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
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
        // PTP::Buddy::BuddyGetALLRes msg;
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
        // request->SendResponseMsg(&msg,CID_BuddyGetALLRes,request->GetSeqNum());
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


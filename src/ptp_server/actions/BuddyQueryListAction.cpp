/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyQueryListAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyQueryListAction.h"
#include "PTP.Buddy.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelBuddy.h"
#include "ImUser.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyQueryListReqAction(CRequest* request){
        PTP::Buddy::BuddyQueryListReq msg; 
        PTP::Buddy::BuddyQueryListRes msg_rsp;
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
             request->Next(&msg,CID_BuddyQueryListReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }

                    list<string> user_ids;
                    if(!msg.params().addresslist().empty()){
                        list<string> addressList;
                        for(const string&address:msg.params().addresslist()){
                            addressList.push_back(address);
                        }
                        CModelBuddy::getUserIdListByAddress(pCacheConn,addressList,user_ids);
                    }

                    if(!msg.params().usernamelist().empty()){
                        auto *buddy = msg_rsp.add_buddy_list();
                        list<string> usernameList;
                        for(const string&username:msg.params().usernamelist()){
                            usernameList.push_back(username);
                        }
                        CModelBuddy::getUserIdListByUserName(pCacheConn,usernameList,user_ids);
                    }

                    if(!user_ids.empty()){
                        for(const string&user_id:user_ids){
                            auto *buddy = msg_rsp.add_buddy_list();
                            CModelBuddy::getUserInfo(pCacheConn,buddy, string_to_int(user_id));
                            CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddress(buddy->address());
                            buddy->set_is_online(pImUser != nullptr);
                        }
                    }

                    auto auth_uid = msg.auth_uid();
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_BuddyQueryListRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_BuddyQueryListRes,request->GetSeqNum());
        }
    }
    void BuddyQueryListResAction(CRequest* request){
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


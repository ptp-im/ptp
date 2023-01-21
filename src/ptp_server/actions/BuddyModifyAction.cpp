/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "BuddyModifyAction.h"
#include "PTP.Buddy.pb.h"
#include "ImUser.h"
#include "MsgSrvConn.h"
#include "models/ModelBuddy.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyModifyReqAction(CRequest* request){
        PTP::Buddy::BuddyModifyReq msg; 
        PTP::Buddy::BuddyModifyRes msg_rsp;
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
             request->Next(&msg,CID_BuddyModifyReq,request->GetSeqNum());
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
                    auto action = msg.buddy_modify_action();
                    auto value = msg.value();
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    msg_rsp.set_value(value);
                    msg_rsp.set_buddy_modify_action(action);
                    CModelBuddy::modify(pCacheConn,action,value,auth_uid,error);
                    if(error == NO_ERROR){
                        list<string> pair_user_ids;
                        CModelGroup::getUserGroupPairs(pCacheConnGroup,pair_user_ids,auth_uid);
                        if(!pair_user_ids.empty()){
                            for(const string &user_id:pair_user_ids){
                                msg_rsp.add_notify_pairs(string_to_int(user_id));
                            }
                        }
                        list<string> group_ids;
                        CModelGroup::getMemberGroupsByStatus(pCacheConnGroup,group_ids,auth_uid,GroupMemberModifyAction_STATUS);
                        if(!group_ids.empty()){
                            for(const string &group_id:group_ids){
                                list<string> group_member_ids;
                                group_member_ids.push_back(to_string(auth_uid));
                                CModelGroup::updateGroupMembersUpdate(
                                        pCacheConnGroup,group_member_ids, string_to_int(group_id),(uint32_t)time(nullptr));
                            }
                        }
                    }
                    break;
                }
                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }

                if (pCacheConnGroup) {
                    pCacheManager->RelCacheConn(pCacheConnGroup);
                }
                request->SendResponseMsg(&msg_rsp,CID_BuddyModifyRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_BuddyModifyRes,request->GetSeqNum());
        }
    }
    void BuddyModifyResAction(CRequest* request){
         PTP::Buddy::BuddyModifyRes msg;
        PTP::Buddy::BuddyModifyNotify msg_notify;
         auto error = msg.error();
         while (true){
             if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
             {
                 error = E_PB_PARSE_ERROR;
                 break;
             }
             uint32_t handle = request->GetHandle();
             auto pMsgConn = FindMsgSrvConnByHandle(handle);
             if(!pMsgConn){
                 DEBUG_E("not found pMsgConn");
                 return;
             }
             PTP::Buddy::BuddyModifyUpdatePairNotify msg_update_pair;

             msg_notify.set_uid(pMsgConn->GetUserId());
             msg_notify.set_value(msg.value());
             msg_notify.set_buddy_modify_action(msg.buddy_modify_action());

             for(unsigned int user_id:msg.notify_pairs()){
                 DEBUG_E("user_id:%d",user_id);
                 CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
                 if(pImUser){
                     ImPdu pdu;
                     pdu.SetPBMsg(&msg_notify,CID_BuddyModifyNotify,0);
                     pImUser->BroadcastPdu(&pdu,pMsgConn);
                 }else{
                     msg_update_pair.add_pair_uid_list(user_id);
                 }
             }
             if(!msg_update_pair.pair_uid_list().empty()){
                 msg_update_pair.set_auth_uid(msg.auth_uid());
                 request->Next(&msg_update_pair,CID_BuddyModifyUpdatePairNotify,0);
             }
             msg.clear_notify_pairs();
             break;
         }
         msg.set_error(error);
         request->SendResponseMsg(&msg,CID_BuddyModifyRes,request->GetSeqNum());
    }
};


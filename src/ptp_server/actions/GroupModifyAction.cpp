/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupModifyAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "ImUser.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupModifyReqAction(CRequest* request){
        PTP::Group::GroupModifyReq msg; 
        PTP::Group::GroupModifyRes msg_rsp;
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
             request->Next(&msg,CID_GroupModifyReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    auto action = msg.group_modify_action();
                    auto value = msg.value();
                    auto group_id = msg.group_id();
                    msg_rsp.set_error(error);
                    msg_rsp.set_value(value);
                    msg_rsp.set_group_modify_action(action);
                    CModelGroup::modify(pCacheConn,action,value,auth_uid,error);
                    if(error == NO_ERROR){
                        list<string> member_ids;
                        CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,group_id,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                        if(!member_ids.empty()){
                            for(const string &user_id:member_ids){
                                msg_rsp.add_notify_members(string2int(user_id));
                            }
                        }
                    }
                    msg_rsp.set_error(error);
                    msg_rsp.set_group_id(msg.group_id());
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_GroupModifyRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupModifyRes,request->GetSeqNum());
        }
    }
    void GroupModifyResAction(CRequest* request){
         PTP::Group::GroupModifyRes msg;
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
             PTP::Group::GroupModifyNotify msg_notify;

             msg_notify.set_value(msg.value());
             msg_notify.set_group_modify_action(msg.group_modify_action());

             for(unsigned int user_id:msg.notify_members()){
                 DEBUG_E("user_id:%d",user_id);
                 CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
                 if(pImUser){
                     ImPdu pdu;
                     pdu.SetPBMsg(&msg_notify,CID_GroupModifyNotify,0);
                     pImUser->BroadcastPdu(&pdu,pMsgConn);
                 }
             }
             msg.clear_notify_members();
             break;
         }
         msg.set_error(error);
         request->SendResponseMsg(&msg,CID_GroupModifyRes,request->GetSeqNum());
    }
};


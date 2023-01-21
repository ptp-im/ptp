/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupChangeMemberAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupChangeMemberAction.h"
#include "PTP.Group.pb.h"
#include "ImUser.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupChangeMemberReqAction(CRequest* request){
        PTP::Group::GroupChangeMemberReq msg; 
        PTP::Group::GroupChangeMemberRes msg_rsp;
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
             request->Next(&msg,CID_GroupChangeMemberReq,request->GetSeqNum());
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

                    list<string> member_ids;
                    uint32_t group_id = msg.group_id();
                    GroupInfo groupInfo;
                    CModelGroup::getGroupInfoById(pCacheConn,&groupInfo,group_id);

                    CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,group_id,0,-1,true);
                    for (auto it = member_ids.begin(); it != member_ids.end(); it++) {
                        string member_id = *it;
                        advance(it,1);
                        string status = *it;
                        if((GroupMemberStatus)string2int(status) == PTP::Common::GROUP_MEMBER_STATUS_NORMAL){
                            if(string2int(member_id) == auth_uid){
                                msg_rsp.add_notify_members(string2int(member_id));
                            }
                        }
                    }

                    auto members = msg.members();
                    list<string> change_member_ids;
                    auto updated_time = time(nullptr);
                    auto group_member_modify_action = msg.group_member_modify_action();

                    if(group_member_modify_action == PTP::Common::GroupMemberModifyAction_ADD){
                        if(groupInfo.group_type() == PTP::Common::GROUP_TYPE_PAIR){
                            error = PTP::Common:: E_SYSTEM;
                            DEBUG_E("GROUP_TYPE_PAIR can not add member");
                            break;
                        }
                        for (unsigned int member_uid : members) {
                            if(!list_string_contains(member_ids, to_string(member_uid))){
                                change_member_ids.push_back(to_string(member_uid));
                                msg_rsp.add_notify_members(member_uid);
                            }
                        }
                        if(!change_member_ids.empty()){
                            CModelGroup::updateGroupMembers(pCacheConn,change_member_ids,group_id,updated_time,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                        }
                    }else if(group_member_modify_action == PTP::Common::GroupMemberModifyAction_STATUS){
                        for (unsigned int member_uid : members) {
                            if(list_string_contains(member_ids, to_string(member_uid))){
                                change_member_ids.push_back(to_string(member_uid));
                            }
                        }
                        if(groupInfo.owner_uid() != auth_uid){
                            error = PTP::Common:: E_SYSTEM;
                            DEBUG_E("del group member has not right");
                            break;
                        }

                        if(!change_member_ids.empty()){
                            CModelGroup::updateGroupMembersStatus(pCacheConn,change_member_ids,group_id,(GroupMemberStatus)msg.group_member_status());
                            CModelGroup::updateGroupMembersUpdate(pCacheConn,change_member_ids,group_id,time(nullptr));
                        }
                    }
                    msg_rsp.set_group_id(group_id);
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_GroupChangeMemberRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupChangeMemberRes,request->GetSeqNum());
        }
    }
    void GroupChangeMemberResAction(CRequest* request){
         PTP::Group::GroupChangeMemberRes msg;
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
             PTP::Group::GroupChangeMemberNotify msg_notify;
             msg_notify.set_group_id(msg.group_id());
             for(unsigned int user_id:msg.notify_members()){
                 DEBUG_E("user_id:%d",user_id);
                 CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
                 if(pImUser){
                     ImPdu pdu;
                     pdu.SetPBMsg(&msg_notify,CID_GroupChangeMemberNotify,0);
                     pImUser->BroadcastPdu(&pdu,pMsgConn);
                 }
             }
             msg.clear_notify_members();
             break;
         }
         msg.set_error(error);
         request->SendResponseMsg(&msg,CID_GroupChangeMemberRes,request->GetSeqNum());
    }
};


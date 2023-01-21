/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupCreateAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupCreateAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_crypto/crypto_helpers.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupCreateReqAction(CRequest* request){
        PTP::Group::GroupCreateReq msg; 
        PTP::Group::GroupCreateRes msg_rsp;
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
             request->Next(&msg,CID_GroupCreateReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
                const auto& sign = msg.sign();
                const auto& captcha = msg.captcha();
                uint32_t group_idx = msg.group_idx();
                string group_name = msg.name();
                string group_avatar = msg.avatar();
                const auto& about = msg.about();
                auto group_type = (PTP::Common::GroupType)msg.group_type();
                auto members = msg.members();

                auto pub_key_64 = recover_pub_key_from_sig(sign, format_sign_msg_data(to_string(group_idx) + captcha,SignMsgType_ptp_group));
                if(pub_key_64.empty()){
                    error = PTP::Common:: E_SYSTEM;
                    DEBUG_E("error pub_key_64 is null");
                    break;
                }
                auto group_address_str = pub_key_to_address(pub_key_64);
                auto group_address = address_to_hex(group_address_str);
                DEBUG_I("group_address: %s",group_address.c_str());

                auto from_uid = msg.auth_uid();
                string from_uid_str = to_string(from_uid);

                uint32_t group_id = CModelGroup::getGroupIdByAddress(pCacheConn,group_address);
                if (group_id > 0) {
                    error = PTP::Common::E_GROUP_HAS_CREATED;
                    break;
                }
                group_id = CModelGroup::genGroupId(pCacheConn);

                if (group_type == PTP::Common::GROUP_TYPE_PAIR) {
                    auto to_uid = members.Get(0);
                    if(CModelGroup::getGroupPairRelation(pCacheConn,from_uid,to_uid) > 0){
                        error = PTP::Common::E_GROUP_CREATE_PAIR_EXISTS;
                        DEBUG_E("GROUP_TYPE_PAIR exists");
                        break;
                    }
                    CModelGroup::updateGroupPairRelation(pCacheConn,group_id,from_uid,to_uid);

                }
                uint32_t created_time = time(nullptr);
                PTP::Common::GroupInfo *group = msg_rsp.mutable_group();
                group->set_group_id(group_id);

                if (group_type == PTP::Common::GROUP_TYPE_PAIR) {
                    auto to_uid = members.Get(0);
                    CModelGroup::updateGroupMemberPairUpdate(pCacheConn,from_uid,to_uid,created_time);
                    CModelGroup::updateGroupMemberPairUpdate(pCacheConn,to_uid,from_uid,created_time);
                    group->set_pair_uid(to_uid);
                }

                string group_id_str     = to_string(group_id);
                string group_idx_str    =  to_string(group_idx);
                string group_type_str   = to_string(group_type);

                list<string> group_member_ids;
                group_member_ids.push_back(from_uid_str);
                PTP::Common::GroupMember* groupMember = msg_rsp.add_group_members();
                groupMember->set_uid(msg.auth_uid());
                groupMember->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);

                for (unsigned int member_uid : members) {
                    if(member_uid != from_uid){
                        PTP::Common::GroupMember* groupMember1 = msg_rsp.add_group_members();
                        group_member_ids.push_back(to_string(member_uid));
                        groupMember1->set_uid((uint32_t)member_uid);
                        groupMember1->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                    }
                }

                CModelGroup::updateGroupMembers(pCacheConn,group_member_ids,group_id,created_time,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);

                group->set_group_id(group_id);
                group->set_group_adr(group_address);
                group->set_group_idx(group_idx);
                group->set_name(group_name);
                group->set_about(about);
                group->set_avatar(group_avatar);
                group->set_updated_time(created_time);
                group->set_created_time(created_time);
                group->set_owner_uid(msg.auth_uid());
                group->set_group_type(group_type);

                list<string> set_argv;
                CModelGroup::cacheGroupInfo(pCacheConn,group);

                pCacheManager->RelCacheConn(pCacheConn);

                msg_rsp.set_auth_uid(from_uid);
                msg_rsp.set_error(error);
                request->SendResponseMsg(&msg_rsp,CID_GroupCreateRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupCreateRes,request->GetSeqNum());
        }
    }
    void GroupCreateResAction(CRequest* request){
         PTP::Group::GroupCreateRes msg;
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
             pMsgConn->SetCaptcha("");
             break;
         }
         msg.set_error(error);
         request->SendResponseMsg(&msg,CID_GroupCreateRes,request->GetSeqNum());
    }
};


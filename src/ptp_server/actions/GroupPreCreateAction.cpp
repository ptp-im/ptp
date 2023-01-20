/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupPreCreateAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupPreCreateAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"
#include "ptp_crypto/crypto_helpers.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupPreCreateReqAction(CRequest* request){
        PTP::Group::GroupPreCreateReq msg; 
        PTP::Group::GroupPreCreateRes msg_rsp;
        ERR error = NO_ERROR;
        request->GetRequestPdu()->Dump();
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
                if (msg.group_type() == GROUP_TYPE_PAIR && msg.members().size() != 1) {
                    error = E_GROUP_CREATE_PAIR_GROUP_MEMBER_SIE_INVALID;
                    break;
                }
                msg.set_auth_uid(pMsgConn->GetUserId());
                request->Next(&msg,CID_GroupPreCreateReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);

                string groupAddress;
                uint32_t groupIndex = 0;

                while (true) {
                    if (!pCacheConn) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }

                    const auto& members = msg.members();
                    auto groupType = msg.group_type();
                    auto fromUserId = msg.auth_uid();
                    auto fromUserId_str = to_string(fromUserId);

                    if (groupType == PTP::Common::GROUP_TYPE_MULTI) {
                        groupIndex = CModelGroup::genGroupIdx(pCacheConn,fromUserId);
                    }

                    if (groupType == PTP::Common::GROUP_TYPE_PAIR) {
                        uint32_t toUserId;
                        string relId;
                        if (members.empty()) {
                            toUserId = fromUserId;
                        } else {
                            toUserId = members.Get(0);
                        }

                        if (toUserId == 0) {
                            error = PTP::Common::E_GROUP_CREATE_PAIR_GROUP_NO_REG_USER;
                            break;
                        }
                        uint32_t group_id;
                        if (toUserId >= fromUserId) {
                            group_id = CModelGroup::getGroupPairRelation(pCacheConn,fromUserId,toUserId);
                        } else {
                            group_id = CModelGroup::getGroupPairRelation(pCacheConn,toUserId,fromUserId);
                        }

                        if (group_id == 0) {
                            groupIndex = CModelGroup::genGroupIdx(pCacheConn,fromUserId);
                            break;
                        } else {
                            GroupInfo group;
                            CModelGroup::getGroupInfoById(pCacheConn,&group,group_id);
                            groupAddress = group.group_adr();
                            groupIndex = group.group_idx();
                        }
                    }
                    msg_rsp.set_auth_uid(fromUserId);
                    break;
                }

                msg_rsp.set_group_adr(groupAddress);
                msg_rsp.set_group_idx(groupIndex);
                msg_rsp.set_error(error);
                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }

                request->SendResponseMsg(&msg_rsp,CID_GroupPreCreateRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupPreCreateRes,request->GetSeqNum());
        }
    }
    void GroupPreCreateResAction(CRequest* request){
         PTP::Group::GroupPreCreateRes msg;
         auto error = msg.error();
         while (true){
             msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength());
             uint32_t handle = request->GetHandle();
             auto pMsgConn = FindMsgSrvConnByHandle(handle);
             if(!pMsgConn){
                 DEBUG_E("not found pMsgConn");
                 return;
             }
             if(error != PTP::Common::NO_ERROR){
                 break;
             }
             string captcha = gen_random(6);
             msg.set_captcha(captcha);
             DEBUG_D("Handle:%d",pMsgConn->GetHandle());
             pMsgConn->SetCaptcha(captcha);
             request->SendResponseMsg(&msg,CID_GroupPreCreateRes,request->GetSeqNum());
             break;
         }
    }
};


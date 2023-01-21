/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetMembersListAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupGetMembersListAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"
#include "models/ModelBuddy.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupGetMembersListReqAction(CRequest* request){
        PTP::Group::GroupGetMembersListReq msg; 
        PTP::Group::GroupGetMembersListRes msg_rsp;
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
             request->Next(&msg,CID_GroupGetMembersListReq,request->GetSeqNum());
            }else{
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);
                CacheConn *pCacheConnAuth = pCacheManager->GetCacheConn(CACHE_AUTH_INSTANCE);
                while (true) {
                    if (!pCacheConn || !pCacheConnAuth) {
                        error = PTP::Common:: E_SYSTEM;
                        DEBUG_E("error pCacheConn");
                        break;
                    }
                    auto auth_uid = msg.auth_uid();
                    auto group_id = msg.group_id();
                    auto group_members_updated_time = msg.group_members_updated_time();
                    list<string> member_ids;
                    CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,group_id,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);

                    for(string &user_id : member_ids){
                        PTP::Common::GroupMember* group_member = msg_rsp.add_group_members();
                        group_member->set_uid(string_to_int(user_id));
                        group_member->set_member_status(PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                    }
                    list<string> updated_member_ids;
                    CModelGroup::getGroupMembersByUpdatedTime(pCacheConn,updated_member_ids,group_id,group_members_updated_time);

                    for(string &user_id : updated_member_ids){
                        PTP::Common::UserInfo * user = msg_rsp.add_members();
                        CModelBuddy::getUserInfo(pCacheConnAuth,user,string_to_int(user_id));
                    }

                    msg_rsp.set_group_id(group_id);
                    msg_rsp.set_group_members_updated_time(time(nullptr));
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                if (pCacheConnAuth) {
                    pCacheManager->RelCacheConn(pCacheConnAuth);
                }
                request->SendResponseMsg(&msg_rsp,CID_GroupGetMembersListRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupGetMembersListRes,request->GetSeqNum());
        }
    }
    void GroupGetMembersListResAction(CRequest* request){
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


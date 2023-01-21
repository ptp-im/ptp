/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadAckAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgReadAckAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelMsg.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReadAckReqAction(CRequest* request){
        PTP::Msg::MsgReadAckReq msg; 
        while (true){
            if(!request->IsBusinessConn()){
                auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
                if(!pMsgConn){
                    DEBUG_E("not found pMsgConn");
                    return;
                }
                msg.set_auth_uid(pMsgConn->GetUserId());
                request->Next(&msg,CID_MsgReadAckReq,request->GetSeqNum());
            }else{
                PTP::Msg::MsgReadNotify msg_notify;
                msg_notify.set_msg_id(msg.msg_id());
                msg_notify.set_group_id(msg.group_id());
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);

                list<string> member_ids;
                CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,msg.group_id(),PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                for(string &user_id:member_ids){
                    if(user_id != to_string(msg.auth_uid())){
                        CModelMsg::updateUnReadMsg(pCacheConn,msg.group_id(),string_to_int(user_id),true);
                        msg_notify.add_notify_users(string_to_int(user_id));
                    }
                }
                msg_notify.set_from_uid(msg.auth_uid());
                request->SendResponseMsg(&msg_notify,CID_MsgReadNotify,request->GetSeqNum());
            }
            break;
        }
        
    }
    
};


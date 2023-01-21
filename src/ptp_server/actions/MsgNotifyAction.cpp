/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgNotifyAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"
#include "ImUser.h"
#include "models/ModelMsg.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgNotifyAction(CRequest* request){
        PTP::Msg::MsgNotify msg;
        while (true){
            if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
            {
                break;
            }
            if(!request->IsBusinessConn()){
                auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
                if(!pMsgConn){
                    DEBUG_E("not found pMsgConn");
                    return;
                }
                auto users = msg.notify_users();
                msg.clear_notify_users();
                for(uint32_t user_id : users){
                    DEBUG_D("notify:%d",user_id);
                    CImUser *pImUser = CImUserManager::GetInstance()->GetImUserById(user_id);
                    if (pImUser) {
                        request->GetRequestPdu()->SetReversed(1);
                        pImUser->BroadcastPdu(request->GetRequestPdu());
                    }
                }
            }else{
                const auto& msg_info = msg.msg_info();
                auto group_id = msg_info.group_id();
                CacheManager *pCacheManager = CacheManager::getInstance();
                CacheConn *pCacheConn = pCacheManager->GetCacheConn(CACHE_GROUP_INSTANCE);

                list<string> member_ids;
                CModelGroup::getGroupMembersByStatus(pCacheConn,member_ids,group_id,PTP::Common::GROUP_MEMBER_STATUS_NORMAL);
                for(string &user_id:member_ids){
                    if(user_id != to_string(msg_info.from_uid())){
                        msg.add_notify_users(string_to_int(user_id));
                        CModelMsg::updateUnReadMsg(pCacheConn,group_id,string_to_int(user_id),false);
                    }
                }
                pCacheManager->RelCacheConn(pCacheConn);
                request->SendResponseMsg(&msg,CID_MsgNotify,0);
            }
            break;
        }
    }
};


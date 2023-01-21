/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupUnreadMsgAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "GroupUnreadMsgAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelGroup.h"
#include "models/ModelMsg.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupUnreadMsgReqAction(CRequest* request){
        PTP::Group::GroupUnreadMsgReq msg; 
        PTP::Group::GroupUnreadMsgRes msg_rsp;
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
             request->Next(&msg,CID_GroupUnreadMsgReq,request->GetSeqNum());
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
                    list<string> unread_msg_list;


                    uint32_t total = 0;
                    CModelMsg::getUnReadMsgList(pCacheConn,auth_uid,total,unread_msg_list);
                    for(string &msg_str : unread_msg_list){
                        PTP::Common::MsgInfo * msg_info = msg_rsp.add_unread_list();
                        msg_info->ParseFromString(hex_to_string(msg_str.substr(2)));

                    }
                    msg_rsp.set_unread_cnt(total);
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_GroupUnreadMsgRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_GroupUnreadMsgRes,request->GetSeqNum());
        }
    }
    void GroupUnreadMsgResAction(CRequest* request){
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


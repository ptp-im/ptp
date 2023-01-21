/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelMsg.h"
#include "models/ModelGroup.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReqAction(CRequest* request){
        PTP::Msg::MsgReq msg; 
        PTP::Msg::MsgRes msg_rsp;
        PTP::Msg::MsgNotify msg_notify;
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
             request->Next(&msg,CID_MsgReq,request->GetSeqNum());
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

                    auto sent_at = msg.sent_at();
                    auto msg_data = msg.msg_data();
                    auto msg_type = msg.msg_type();
                    const auto& group_adr = msg.group_adr();

                    PTP::Common::MsgInfo* msg_info = msg_notify.mutable_msg_info();
                    uint32_t group_id = CModelGroup::getGroupIdByAddress(pCacheConn,group_adr);
                    if (group_id == 0) {
                        error = PTP::Common::E_SYSTEM;
                        DEBUG_E("not found group");
                        break;
                    }
                    msg_rsp.set_group_id(group_id);
                    uint32_t msg_id = CModelMsg::genMsgId(pCacheConn,group_id);
                    msg_rsp.set_msg_id(msg_id);
                    msg_rsp.set_sent_at(sent_at);
                    string msg_id_str = to_string(msg_id);
                    auto fromUserId = auth_uid;
                    string fromUserId_str = to_string(fromUserId);
                    msg_info->set_msg_id(msg_id);
                    msg_info->set_msg_data(msg_data);
                    msg_info->set_from_uid(fromUserId);
                    msg_info->set_sent_at(sent_at);
                    msg_info->set_group_id(group_id);
                    msg_info->set_msg_type(msg_type);
                    CModelMsg::saveMsgInfo(pCacheConn,msg_info);
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_MsgRes,request->GetSeqNum());
                request->Next(&msg_notify,CID_MsgNotify,0);
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_MsgRes,request->GetSeqNum());
        }
    }
    void MsgResAction(CRequest* request){
        // PTP::Msg::MsgRes msg;
        // auto error = msg.error();
        // while (true){
        //     if(!msg.ParseFromArray(request->GetRequestPdu()->GetBodyData(), (int)request->GetRequestPdu()->GetBodyLength()))
        //     {
        //         error = E_PB_PARSE_ERROR;
        //         break;
        //     }
        //     if(!request->IsBusinessConn()){
        //       uint32_t handle = request->GetHandle();
        //       auto pMsgConn = FindMsgSrvConnByHandle(handle);
        //       if(!pMsgConn){
        //           DEBUG_E("not found pMsgConn");
        //           return;
        //       }
        //       if(error != PTP::Common::NO_ERROR){
        //           break;
        //       }
        //     }else{
        //       if(error != PTP::Common::NO_ERROR){
        //           break;
        //       }
        //     }
        //     break;
        // }
        // msg.set_error(error);
        // request->SendResponseMsg(&msg,CID_MsgRes,request->GetSeqNum());
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


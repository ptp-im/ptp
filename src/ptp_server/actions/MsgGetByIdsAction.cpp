/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetByIdsAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "MsgGetByIdsAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelMsg.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgGetByIdsReqAction(CRequest* request){
        PTP::Msg::MsgGetByIdsReq msg; 
        PTP::Msg::MsgGetByIdsRes msg_rsp;
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
             request->Next(&msg,CID_MsgGetByIdsReq,request->GetSeqNum());
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
                    list<string> msg_str_list;
                    list<uint32_t> msg_ids;

                    for(uint32_t msg_id : msg.msg_ids()){
                        msg_ids.push_back(msg_id);
                    }
                    CModelMsg::getMsgListById(pCacheConn,msg.group_id(),msg_ids,msg_str_list);

                    for(string& msg_str : msg_str_list){
                        string msg_buf = hex_to_string(msg_str.substr(2));
                        auto msg_info = msg_rsp.add_msg_list();
                        msg_info->ParseFromArray(msg_buf.data(), msg_buf.length());
                    }
                    msg_rsp.set_error(error);
                    msg_rsp.set_auth_uid(auth_uid);
                    break;
                }

                if (pCacheConn) {
                    pCacheManager->RelCacheConn(pCacheConn);
                }
                request->SendResponseMsg(&msg_rsp,CID_MsgGetByIdsRes,request->GetSeqNum());
            }
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            request->SendResponseMsg(&msg_rsp,CID_MsgGetByIdsRes,request->GetSeqNum());
        }
    }
    void MsgGetByIdsResAction(CRequest* request){
        // PTP::Msg::MsgGetByIdsRes msg;
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
        // request->SendResponseMsg(&msg,CID_MsgGetByIdsRes,request->GetSeqNum());
        request->SendResponsePdu(request->GetRequestPdu());
    }
};


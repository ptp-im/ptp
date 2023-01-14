/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "MsgAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReqAction(CRequest* request, CResponse *response){
        PTP::Msg::MsgReq msg; 
        PTP::Msg::MsgRes msg_rsp;
        ERR error = NO_ERROR;
        if(!request->IsBusinessConn()){
          auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
          if(!pMsgConn){
              DEBUG_E("not found pMsgConn");
              return;
          }
        }
        while (true){
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            //msg.set_auth_id(pMsgConn->GetUserId());
            //response->Next(&msg,CID_MsgReq,request->GetPdu()->GetSeqNum());
            //response->SendMsg(&msg_rsp,CID_MsgRes,request->GetPdu()->GetSeqNum());
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            response->SendMsg(&msg_rsp,CID_MsgRes,request->GetPdu()->GetSeqNum());
        }
    }
    void MsgResAction(CRequest* request, CResponse *response){
        PTP::Msg::MsgRes msg;
        auto error = msg.error();
        while (true){
            if(!request->IsBusinessConn()){
              uint32_t handle = request->GetHandle();
              auto pMsgConn = FindMsgSrvConnByHandle(handle);
              if(!pMsgConn){
                  DEBUG_E("not found pMsgConn");
                  return;
              }
            }
            if(error != PTP::Common::NO_ERROR){
                break;
            }
            if(!msg.ParseFromArray(request->GetPdu()->GetBodyData(), (int)request->GetPdu()->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            break;
        }
        msg.set_error(error);
        response->SendMsg(&msg,CID_MsgRes,request->GetPdu()->GetSeqNum());
    }
};


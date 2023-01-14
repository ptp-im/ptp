/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "GroupRemoveSessionAction.h"
#include "PTP.Group.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_GROUP {
    void GroupRemoveSessionReqAction(CRequest* request, CResponse *response){
        PTP::Group::GroupRemoveSessionReq msg; 
        PTP::Group::GroupRemoveSessionRes msg_rsp;
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
            //response->Next(&msg,CID_GroupRemoveSessionReq,request->GetPdu()->GetSeqNum());
            //response->SendMsg(&msg_rsp,CID_GroupRemoveSessionRes,request->GetPdu()->GetSeqNum());
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            response->SendMsg(&msg_rsp,CID_GroupRemoveSessionRes,request->GetPdu()->GetSeqNum());
        }
    }
    void GroupRemoveSessionResAction(CRequest* request, CResponse *response){
        PTP::Group::GroupRemoveSessionRes msg;
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
        response->SendMsg(&msg,CID_GroupRemoveSessionRes,request->GetPdu()->GetSeqNum());
    }
};


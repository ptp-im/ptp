/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModifyAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "BuddyModifyAction.h"
#include "PTP.Buddy.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_BUDDY {
    void BuddyModifyReqAction(CRequest* request, CResponse *response){
        PTP::Buddy::BuddyModifyReq msg; 
        PTP::Buddy::BuddyModifyRes msg_rsp;
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
            //response->Next(&msg,CID_BuddyModifyReq,request->GetPdu()->GetSeqNum());
            //response->SendMsg(&msg_rsp,CID_BuddyModifyRes,request->GetPdu()->GetSeqNum());
            break;
        }
        if(error!= NO_ERROR){
            msg_rsp.set_error(error);
            response->SendMsg(&msg_rsp,CID_BuddyModifyRes,request->GetPdu()->GetSeqNum());
        }
    }
    void BuddyModifyResAction(CRequest* request, CResponse *response){
        PTP::Buddy::BuddyModifyRes msg;
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
        response->SendMsg(&msg,CID_BuddyModifyRes,request->GetPdu()->GetSeqNum());
    }
};


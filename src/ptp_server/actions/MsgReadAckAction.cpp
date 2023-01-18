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

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReadAckReqAction(CRequest* request){
        PTP::Msg::MsgReadAckReq msg; 
        
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
             request->Next(&msg,CID_MsgReadAckReq,request->GetSeqNum());
            }else{
            }
            break;
        }
        
    }
    
};


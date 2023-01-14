/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadAckAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "MsgReadAckAction.h"
#include "PTP.Msg.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_MSG {
    void MsgReadAckReqAction(CRequest* request, CResponse *response){
        PTP::Msg::MsgReadAckReq msg; 
        
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
            //response->Next(&msg,CID_MsgReadAckReq,request->GetPdu()->GetSeqNum());
            break;
        }
        
    }
    
};


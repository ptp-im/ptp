/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  CaptchaAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "CaptchaAction.h"
#include "PTP.Other.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelOther.h"

using namespace PTP::Common;

namespace ACTION_OTHER {
    void CaptchaReqAction(CRequest* request){
        PTP::Other::CaptchaReq msg; 
        
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
            }else{
            }
            break;
        }
        
    }
    
};


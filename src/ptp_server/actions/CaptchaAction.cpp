/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  CaptchaAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "CaptchaAction.h"
#include "PTP.Other.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_OTHER {
    void CaptchaReqAction(CRequest* request, CResponse *response){
        PTP::Other::CaptchaReq msg; 
        
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
            break;
        }
        
    }
    
};


/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthCaptchaAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "AuthCaptchaAction.h"
#include "PTP.Auth.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_AUTH {
    void AuthCaptchaReqAction(CRequest* request, CResponse *response){
        PTP::Auth::AuthCaptchaReq msg; 
        
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


/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLogoutAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "AuthLogoutAction.h"
#include "PTP.Auth.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_AUTH {
    void AuthLogoutReqAction(CRequest* request){
        PTP::Auth::AuthLogoutReq msg;
        ERR error = NO_ERROR;
        if(!request->IsBusinessConn()){
          auto pMsgConn = FindMsgSrvConnByHandle(request->GetHandle());
          if(!pMsgConn){
              DEBUG_E("not found pMsgConn");
              return;
          }
          pMsgConn->Close();
        }
    }
};

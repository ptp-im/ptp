/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc：
 *
================================================================*/
#include "SwitchDevicesAction.h"
#include "PTP.Switch.pb.h"
#include "MsgSrvConn.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchDevicesReqAction(CRequest* request, CResponse *response){
        PTP::Switch::SwitchDevicesReq msg; 
        
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


/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesAction.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
================================================================*/
#include "SwitchDevicesAction.h"
#include "PTP.Switch.pb.h"
#include "MsgSrvConn.h"
#include "models/ModelSwitch.h"

using namespace PTP::Common;

namespace ACTION_SWITCH {
    void SwitchDevicesReqAction(CRequest* request){
        PTP::Switch::SwitchDevicesReq msg; 
        
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


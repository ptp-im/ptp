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
                DEBUG_D("client_id=%s ", msg.client_id().c_str());
                DEBUG_D("browser_name=%s ", msg.browser_name().c_str());
                DEBUG_D("browser_version=%s ", msg.browser_version().c_str());
                DEBUG_D("os_name=%s ", msg.os_name().c_str());
                DEBUG_D("os_version=%s ", msg.os_version().c_str());
                DEBUG_D("is_intel=%d ", msg.is_intel());
                pMsgConn->m_is_intel = msg.is_intel();
                pMsgConn->m_client_id = msg.client_id();
                pMsgConn->m_os_name = msg.os_name();
                pMsgConn->m_os_version = msg.os_version();
                pMsgConn->m_browser_name = msg.browser_name();
                pMsgConn->m_browser_version = msg.browser_version();
            }
            break;
        }
        
    }
    
};


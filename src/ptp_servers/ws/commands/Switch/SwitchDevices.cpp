/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevices.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, November 7, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"

using namespace PTP::Common;

namespace COMMAND {
    void SwitchDevicesReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("SwitchDevicesReq start...");
        PTP::Switch::SwitchDevicesReq msg; 
        PTP::Switch::SwitchDevicesNotify msg_notify;
        DEBUG_I("conn_uuid=%u", conn_uuid);

        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            DEBUG_E("not found pMsgConn");
            return;
        }

        if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            return;
        }
        auto pImUser = CImUserManager::GetInstance()->GetImUserById(pMsgConn->GetUserId());

        DEBUG_D("client_id=%s ", msg.client_id().c_str());
        DEBUG_D("browser_name=%s ", msg.browser_name().c_str());
        DEBUG_D("browser_version=%s ", msg.browser_version().c_str());
        DEBUG_D("os_name=%s ", msg.os_name().c_str());
        DEBUG_D("os_version=%s ", msg.os_version().c_str());

        pMsgConn->SetClientInfo(
                msg.client_id(),msg.is_intel(),
                msg.os_name(),msg.os_version(),
                msg.browser_name(),msg.browser_version());

        pImUser->BroadcastDevicesPdu();
    }

};


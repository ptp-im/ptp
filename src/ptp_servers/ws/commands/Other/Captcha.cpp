/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  Captcha.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Friday, December 2, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void CaptchaReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("CaptchaReq start...");
        PTP::Other::CaptchaReq msg; 
        
        DEBUG_I("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;
        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            DEBUG_E("not found pMsgConn");
            return;
        }
        while (true){
            if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }
            
            CDBServConn* pDbConn = get_db_serv_conn();
            if (!pDbConn) {
                error = E_REASON_NO_DB_SERVER;
                DEBUG_E("not found pDbConn");
                break;
            }
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
        
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        DEBUG_D("CaptchaReq end...");
    }
    
};


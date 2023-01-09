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

#include "util.h"
#include "ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Other.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void CaptchaReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        log_debug("CaptchaReq start...");
        PTP::Other::CaptchaReq msg; 
        
        log("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;
        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            log_error("not found pMsgConn");
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
                log_error("not found pDbConn");
                break;
            }
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
        
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        log_debug("CaptchaReq end...");
    }
    
};


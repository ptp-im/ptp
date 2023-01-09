/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadAck.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Tuesday, November 8, 2022
 *   desc：
 *
================================================================*/

#include "util.h"
#include "ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Msg.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void MsgReadAckReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        log_debug("MsgReadAckReq start...");
        PTP::Msg::MsgReadAckReq msg; 
        
        log("conn_uuid=%u", conn_uuid);

        auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
        if(!pMsgConn){
            log_error("not found pMsgConn");
            return;
        }
        ERR error = NO_ERROR;
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
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            msg.set_auth_uid(pMsgConn->GetUserId());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
        
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        log_debug("MsgReadAckReq end...");
    }
    
};


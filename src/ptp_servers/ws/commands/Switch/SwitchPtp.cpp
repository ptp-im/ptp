/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtp.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, November 6, 2022
 *   desc：
 *
================================================================*/

#include "util.h"
#include "ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Switch.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void SwitchPtpReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        log_debug("SwitchPtpReq start...");
        PTP::Switch::SwitchPtpReq msg; 
        PTP::Switch::SwitchPtpRes msg_rsp;
        PTP::Switch::SwitchPtpNotify msg_notify;
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

            string to_adr = msg.to_adr();
            auto toUser = CImUserManager::GetInstance()->GetImUserByAddressHex(to_adr);
            msg_notify.set_from_adr(pMsgConn->GetAddressHex());
            msg_notify.set_data(msg.data());
            msg_notify.set_switch_type(msg.switch_type());
            CImPdu pdu;
            pdu.SetPBMsg(&msg_notify);
            pdu.SetServiceId(S_SWITCH);
            pdu.SetCommandId(CID_SwitchPtpNotify);
            pdu.SetSeqNum(0);
            if(toUser){
                toUser->BroadcastPdu(&pdu);
            }else{
                error = PTP::Common::E_SWITCH_USER_NO_ONLINE;
            }
            break;
        }


        msg_rsp.set_error(error);

        CImPdu pdu;
        pdu.SetPBMsg(&msg_rsp);
        pdu.SetServiceId(S_SWITCH);
        pdu.SetCommandId(CID_SwitchPtpRes);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        pMsgConn->SendPdu(&pdu);
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        log_debug("SwitchPtpReq end...");
    }
    
    void SwitchPtpResCmd(CImPdu* pPdu){
        //ignore
    }
};


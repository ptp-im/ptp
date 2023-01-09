/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgUnNotify.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
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
    void MsgUnNotifyCmd(CImPdu* pPdu){
        log_debug("MsgUnNotify start...");
        PTP::Msg::MsgUnNotify msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            // CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            // uint32_t conn_uuid = attach_data.GetHandle();
            // msg.clear_attach_data();
            // log("conn_uuid=%u", conn_uuid);
            // auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            // if(!pMsgConn){
            //     log_error("not found pMsgConn");
            //     return;
            // }
            // CImPdu pdu_rsp;
            // pdu_rsp.SetPBMsg(&msg);
            // pdu_rsp.SetServiceId(PTP::Common::S_MSG);
            // pdu_rsp.SetCommandId(PTP::Common::CID_MsgUnNotify);
            // pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            // pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        log_debug("MsgUnNotify end...");
    }
};


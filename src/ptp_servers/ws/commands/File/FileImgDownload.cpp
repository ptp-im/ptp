/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgDownload.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc：
 *
================================================================*/

#include "ptp_global/Utils.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void FileImgDownloadReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("FileImgDownloadReq start...");
        PTP::File::FileImgDownloadReq msg; 
        PTP::File::FileImgDownloadRes msg_rsp;
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
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_auth_uid(pMsgConn->GetUserId());
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);
            break;
        }
        if(error!= NO_ERROR){
            
            msg_rsp.set_error(error);
                            
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_FILE);
            pdu.SetCommandId(CID_FileImgDownloadRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
        }
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        DEBUG_D("FileImgDownloadReq end...");
    }
    void FileImgDownloadResCmd(CImPdu* pPdu){
        DEBUG_D("FileImgDownloadRes start...");
        PTP::File::FileImgDownloadRes msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t conn_uuid = attach_data.GetHandle();
            msg.clear_attach_data();
            DEBUG_I("conn_uuid=%u", conn_uuid);
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }
            CImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_FILE);
            pdu_rsp.SetCommandId(PTP::Common::CID_FileImgDownloadRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("FileImgDownloadRes end...");
    }
};

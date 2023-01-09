/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  FileImgUpload.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.File.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void FileImgUploadReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("FileImgUploadReq start...");
        PTP::File::FileImgUploadReq msg; 
        PTP::File::FileImgUploadRes msg_rsp;
        DEBUG_I("conn_uuid=%u", conn_uuid);
        ERR error = NO_ERROR;
        while (true){
            if(!msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
            {
                error = E_PB_PARSE_ERROR;
                break;
            }

            auto file_id = msg.file_id();
            auto file_part = msg.file_part();
            auto file_total_parts = msg.file_total_parts();
            auto file_data = msg.file_data();
            msg_rsp.set_file_path("ww");
            msg_rsp.set_error(NO_ERROR);
            break;
        }
   
        if(error!= NO_ERROR){
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            msg_rsp.set_error(error);
                            
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_FILE);
            pdu.SetCommandId(CID_FileImgUploadRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("FileImgUploadReq end...");
    }
    
    void FileImgUploadResCmd(CImPdu* pPdu){
        DEBUG_D("FileImgUploadRes start...");
        PTP::File::FileImgUploadRes msg;
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
            pdu_rsp.SetCommandId(PTP::Common::CID_FileImgUploadRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("FileImgUploadRes end...");
    }
};


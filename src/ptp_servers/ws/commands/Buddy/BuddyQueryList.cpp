/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyQueryList.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Thursday, December 29, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Util.h"
#include "ptp_global/ImPduBase.h"
#include "ImUser.h"
#include "AttachData.h"
#include "PTP.Buddy.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void BuddyQueryListReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("BuddyQueryListReq start...");
        PTP::Buddy::BuddyQueryListReq msg; 
        PTP::Buddy::BuddyQueryListRes msg_rsp;
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
            pdu.SetServiceId(S_BUDDY);
            pdu.SetCommandId(CID_BuddyQueryListRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
        }
        //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        DEBUG_D("BuddyQueryListReq end...");
    }
    void BuddyQueryListResCmd(CImPdu* pPdu){
        DEBUG_D("BuddyQueryListRes start...");
        PTP::Buddy::BuddyQueryListRes msg;
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
            if(!msg.buddy_list().empty()){
                PTP::Buddy::BuddyQueryListRes msg1;
                msg1.set_error(msg.error());
                for(const auto & it : msg.buddy_list()){
                    PTP::Common::UserInfo * user = msg1.add_buddy_list();
                    user->set_uid(it.uid());
                    user->set_user_name(it.user_name());
                    user->set_pub_key(it.pub_key());
                    user->set_address(it.address());
                    user->set_nick_name(it.nick_name());
                    user->set_first_name(it.first_name());
                    user->set_last_name(it.last_name());
                    user->set_sign_info(it.sign_info());
                    user->set_status(it.status());
                    user->set_avatar(it.avatar());
                    user->set_login_time(it.login_time());
                    CImUser *pImUser = CImUserManager::GetInstance()->GetImUserByAddressHex(it.address());
                    if (!pImUser) {
                        user->set_is_online(false);
                    }else{
                        user->set_is_online(true);
                    }
                }
                pdu_rsp.SetPBMsg(&msg1);
            }else{
                pdu_rsp.SetPBMsg(&msg);
            }
            pdu_rsp.SetServiceId(PTP::Common::S_BUDDY);
            pdu_rsp.SetCommandId(PTP::Common::CID_BuddyQueryListRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);
            break;
        }
        DEBUG_D("BuddyQueryListRes end...");
    }
};


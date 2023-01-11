/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModify.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
================================================================*/

#include "ptp_global/Utils.h"
#include "ptp_protobuf/ImPdu.h"
#include "ImUser.h"
#include "ptp_global/AttachData.h"
#include "PTP.Group.pb.h"

using namespace PTP::Common;

namespace COMMAND {
    void GroupModifyReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
        DEBUG_D("GroupModifyReq start...");
        PTP::Group::GroupModifyReq msg; 
        PTP::Group::GroupModifyRes msg_rsp;
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
            CBusinessServConn* pDbConn = get_business_serv_conn();
            if (!pDbConn) {
                error = E_REASON_NO_DB_SERVER;
                DEBUG_E("not found pDbConn");
                break;
            }
            CDbAttachData attach(ATTACH_TYPE_HANDLE, conn_uuid, 0);
            msg.set_attach_data(attach.GetBuffer(), attach.GetLength());
            msg.set_auth_uid(pMsgConn->GetUserId());
            pPdu->SetPBMsg(&msg);
            pDbConn->SendPdu(pPdu);

//
//void _Handle_GroupModifyReq(CImPdu *pPdu, CMsgConn * pMsgConn)
//{
//    DEBUG_D("_Handle_GroupModifyReq start...");
//    PTP::Group::GroupModifyReq msg;
//    PTP::Group::GroupModifyNotify msg2;
//    PTP::Group::GroupModifyRes msg1;
//    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
//    PTP::Common::ERR error = NO_ERROR;
//    CacheManager *pCacheManager = CacheManager::getInstance();
//    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");
//    while (1) {
//        if (!pCacheConn) {
//            error = E_SYSTEM;
//            DEBUG_E("error pCacheConn");
//            break;
//        }
//        string value = msg.value();
//        auto group_modify_action = msg.group_modify_action();
//        uint32_t group_id = msg.group_id();
//        string group_id_str = to_string(group_id);
//        string groupType_str = pCacheConn->get(GROUP_TYPE_PREFIX + group_id_str);
//        auto groupType = (PTP::Common::GroupType) atoi(groupType_str.c_str());
//        if(groupType == GROUP_TYPE_PAIR){
//            error = E_SYSTEM;
//            DEBUG_E("pair group can not modify");
//            break;
//        }
//
//        string group_info_hex = pCacheConn->get(GROUP_INFO_PREFIX + group_id_str);
//        string group_info_buf = hex_to_string(group_info_hex.substr(2));
//        PTP::Common::GroupInfo group;
//        group.ParseFromArray(group_info_buf.data(), group_info_buf.length());
//
//        auto fromUserId = pMsgConn->GetUserId();
//        auto fromUserId_str = to_string(fromUserId);
//        auto fromAddress = pMsgConn->GetAddressHex();
//
//        if(group.owner_uid() != fromUserId){
//            error = E_SYSTEM;
//            DEBUG_E("you are not owner to modify group");
//            break;
//        }
//
//        switch (group_modify_action) {
//            case GroupModifyAction_avatar:
//                group.set_avatar(value);
//                break;
//            case GroupModifyAction_name:
//                group.set_name(value);
//                break;
//            default:
//                break;
//        }
//
//        string group_ = group.SerializeAsString();
//        string group_info_string = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(group_.data()), group_.size());
//
//        uint32_t updated_time = time(nullptr);
//
//        list<string> argv_set;
//        argv_set.emplace_back("MSET");
//        argv_set.emplace_back(GROUP_INFO_PREFIX + group_id_str);
//        argv_set.emplace_back(group_info_string);
//        argv_set.emplace_back(GROUP_UPDATE_PREFIX + group_id_str);
//        argv_set.emplace_back(to_string(updated_time));
//        pCacheConn->exec(&argv_set, nullptr);
//
//        list<string> members;
//        list<string> argv;
//        argv.emplace_back("0");
//        argv.emplace_back("-1");
//
//        pCacheConn->cmd("ZRANGE", GROUP_MEMBER_STATUS_KEY_PREFIX + group_id_str, &argv, &members);
//
//        if (members.size() > 0) {
//            msg2.set_group_id(group_id);
//            msg2.set_value(value);
//            msg2.set_group_modify_action(group_modify_action);
//            ImPdu pdu2;
//            pdu2.SetPBMsg(&msg2);
//            pdu2.SetServiceId(S_GROUP);
//            pdu2.SetCommandId(CID_GroupModifyNotify);
//            pdu2.SetSeqNum(0);
//            for (auto it = members.begin(); it != members.end(); it++) {
//                string toUserId_str = it->c_str();
//                uint32_t toUserId = (uint32_t) (atoi(toUserId_str.c_str()));
//                DEBUG_D("CID_GroupModifyNotify: %d -> %d",fromUserId,toUserId);
//                CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
//                if (pToImUser) {
//                    pToImUser->BroadcastPdu(&pdu2, pMsgConn);
//                }
//            }
//        }
//        msg1.set_group_id(group_id);
//        msg1.set_group_modify_action(group_modify_action);
//        break;
//    }
//
//    if (pCacheConn) {
//        pCacheManager->RelCacheConn(pCacheConn);
//    }
//    msg1.set_error(error);
//    ImPdu pdu;
//    pdu.SetPBMsg(&msg1);
//    pdu.SetServiceId(S_GROUP);
//    pdu.SetCommandId(CID_GroupModifyRes);
//    pdu.SetSeqNum(pPdu->GetSeqNum());
//    pMsgConn->SendPdu(&pdu);
//    DEBUG_D("_Handle_GroupModifyReq end.");
//}
            break;
        }
   
        if(error!= NO_ERROR){
            
            msg_rsp.set_error(error);
                            
            ImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_GROUP);
            pdu.SetCommandId(CID_GroupModifyRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu);
            //CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
        }
        DEBUG_D("GroupModifyReq end...");
    }
    
    void GroupModifyResCmd(CImPdu* pPdu){
        DEBUG_D("GroupModifyRes start...");
        PTP::Group::GroupModifyRes msg;
        CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
        while (true){
            CDbAttachData attach_data((uchar_t*)msg.attach_data().c_str(), msg.attach_data().length());
            uint32_t conn_uuid = attach_data.GetHandle();
            msg.clear_attach_data();
            auto notify_members = msg.notify_members();
            msg.clear_notify_members();
            DEBUG_I("conn_uuid=%u", conn_uuid);
            auto pMsgConn = FindWebSocketConnByHandle(conn_uuid);
            if(!pMsgConn){
                DEBUG_E("not found pMsgConn");
                return;
            }

            ImPdu pdu_rsp;
            pdu_rsp.SetPBMsg(&msg);
            pdu_rsp.SetServiceId(PTP::Common::S_GROUP);
            pdu_rsp.SetCommandId(PTP::Common::CID_GroupModifyRes);
            pdu_rsp.SetSeqNum(pPdu->GetSeqNum());
            pMsgConn->SendPdu(&pdu_rsp);

            PTP::Group::GroupModifyNotify msg_notify;
            msg_notify.set_value(msg.value());
            msg_notify.set_group_modify_action(msg.group_modify_action());
            ImPdu pdu2;

            pdu2.SetPBMsg(&msg_notify);
            pdu2.SetServiceId(PTP::Common::S_GROUP);
            pdu2.SetCommandId(PTP::Common::CID_GroupModifyNotify);
            pdu2.SetSeqNum(0);

            for (auto it = notify_members.begin(); it != notify_members.end(); it++) {
                uint32_t toUserId = *it;
                CImUser *pToImUser = CImUserManager::GetInstance()->GetImUserById(toUserId);
                if (pToImUser && toUserId != msg.auth_uid()) {
                    DEBUG_I("CID_GroupModifyNotify: %d -> %d",msg.auth_uid(),toUserId);
                    pToImUser->BroadcastPdu(&pdu2, pMsgConn);
                }
            }

            break;
        }
        DEBUG_D("GroupModifyRes end...");
    }
};


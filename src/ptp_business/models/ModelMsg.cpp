/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Saturday, November 5, 2022
 *   desc：
 *
 *
================================================================*/
#include "ModelMsg.h"
#include "CachePool.h"

#include "helpers.h"
CModelMsg* CModelMsg::m_pInstance = NULL;

CModelMsg::CModelMsg()
{
    
}

CModelMsg::~CModelMsg()
{
    
}

CModelMsg* CModelMsg::getInstance()
{
    if (m_pInstance == NULL) {
        m_pInstance = new CModelMsg();
    }
    return m_pInstance;
}


void CModelMsg::MsgReq(PTP::Msg::MsgReq* msg, PTP::Msg::MsgRes* msg_rsp,PTP::Msg::MsgNotify* msg_notify, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    auto sent_at = msg->sent_at();
    auto msg_data = msg->msg_data();
    auto msg_type = msg->msg_type();
    auto group_adr = msg->group_adr();

    PTP::Common::MsgInfo* msg_info = msg_notify->mutable_msg_info();

    auto created_time = time(nullptr);


    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        auto group_id_str = pCacheConn->get(GROUP_ADDRESS_ID_PREFIX + group_adr);
        if (group_id_str.empty()) {
            error = PTP::Common::E_SYSTEM;
            log_error("not found group");
            break;
        }
        uint32_t group_id = string2int(group_id_str);
        msg_rsp->set_group_id(group_id);
        long msg_id_long = pCacheConn->incrBy(GROUP_MSG_ID_BY_ID_INCR_PREFIX + group_id_str, 1);

        uint32_t msg_id = (uint32_t) msg_id_long;
        msg_rsp->set_msg_id(msg_id);
        msg_rsp->set_sent_at(sent_at);
        string msg_id_str = to_string(msg_id);
        auto fromUserId = msg->auth_uid();
        string fromUserId_str = to_string(fromUserId);
        msg_info->set_msg_id(msg_id);
        msg_info->set_msg_data(msg_data);
        msg_info->set_from_uid(fromUserId);
        msg_info->set_sent_at(sent_at);
        msg_info->set_group_id(group_id);
        msg_info->set_msg_type(msg_type);

        string msg_serialize = msg_info->SerializeAsString();
        string msg_info_string = bytes_to_hex_string(reinterpret_cast<const uint8_t *>(msg_serialize.data()), msg_serialize.size());

        list<string> argv_set;
        argv_set.emplace_back("MSET");
        argv_set.push_back(GROUP_MSG_DATA_BY_ID_PREFIX + group_id_str + "_" + msg_id_str);
        argv_set.push_back(msg_info_string);
        argv_set.push_back(GROUP_LATEST_MSG_DATA_BY_ID_PREFIX + group_id_str);
        argv_set.push_back(msg_info_string);
        argv_set.push_back(GROUP_MSG_UPDATE_PREFIX + group_id_str);
        argv_set.push_back(to_string(created_time));
        pCacheConn->exec(&argv_set, nullptr);
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void CModelMsg::MsgReadAckReq(PTP::Msg::MsgReadAckReq* msg, PTP::Msg::MsgReadNotify* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        uint32_t group_id = msg->group_id();
        auto msg_id = msg->msg_id();

        uint32_t user_id = msg->auth_uid();
        string user_id_str = to_string(user_id);

        string group_id_str = to_string(group_id);
        string msg_info_hex = pCacheConn->get(GROUP_MSG_DATA_BY_ID_PREFIX + group_id_str + "_" + to_string(msg_id));

        PTP::Common::MsgInfo msg_info;
        msg_info.ParseFromString(hex_to_string(msg_info_hex.substr(2)));
        msg_rsp->set_group_id(group_id);
        msg_rsp->set_msg_id(msg_id);
        msg_rsp->set_from_uid(msg->auth_uid());
        list<string> argv_unread;
        argv_unread.emplace_back("ZADD");
        argv_unread.emplace_back(MEMBER_GROUP_UNREAD_KEY_PREFIX+ user_id_str);
        argv_unread.emplace_back("0");
        argv_unread.emplace_back(group_id_str);
        pCacheConn->exec(&argv_unread, nullptr);
        log("clear unread %d",user_id);
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}
void CModelMsg::MsgGetMaxIdReq(PTP::Msg::MsgGetMaxIdReq* msg, PTP::Msg::MsgGetMaxIdRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        string group_id_str = to_string(msg->group_id());
        string msg_id_str = pCacheConn->get(GROUP_MSG_ID_BY_ID_INCR_PREFIX + group_id_str);
        msg_rsp->set_msg_id(string2int(msg_id_str));
        msg_rsp->set_group_id(msg->group_id());
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelMsg::MsgGetByIdsReq(PTP::Msg::MsgGetByIdsReq* msg, PTP::Msg::MsgGetByIdsRes* msg_rsp, PTP::Common::ERR& error){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            error = PTP::Common:: E_SYSTEM;
            log_error("error pCacheConn");
            break;
        }
        string group_id_str = to_string(msg->group_id());
        list<string> argv_msg_list;
        list<string> ret_msg_list;
        argv_msg_list.emplace_back("MGET");
        for (auto it = msg->msg_ids().begin(); it != msg->msg_ids().end(); it++) {
            argv_msg_list.push_back(GROUP_MSG_DATA_BY_ID_PREFIX + group_id_str + "_" + to_string(*it));
        }

        pCacheConn->exec(&argv_msg_list,&ret_msg_list);

        list<PTP::Common::MsgInfo> msg_list;
        for (auto it = ret_msg_list.begin(); it != ret_msg_list.end(); it++) {
            string msg_hex = it->c_str();
            if(!msg_hex.empty()){
                string msg_buf = hex_to_string(msg_hex.substr(2));
                auto msg_info = msg_rsp->add_msg_list();
                msg_info->ParseFromArray(msg_buf.data(), msg_buf.length());
            }
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

void CModelMsg::MsgUnNotify(PTP::Msg::MsgUnNotify* msg){
    CacheManager *pCacheManager = CacheManager::getInstance();
    CacheConn *pCacheConn = pCacheManager->GetCacheConn("group");

    while (true) {
        if (!pCacheConn) {
            log_error("error pCacheConn");
            break;
        }
        string group_id_str = to_string(msg->group_id());
        if(msg->un_notify_users().size() > 0){
            for(auto it = msg->un_notify_users().begin();it != msg->un_notify_users().end();it++){
                uint32_t toUserId = *it;
                list<string> argv1;
                argv1.emplace_back("1");
                argv1.push_back(group_id_str);
                if(toUserId!= msg->from_uid()){
                    log_debug("incr unread,toUserId:%d, group:%s",toUserId,group_id_str.c_str());
                    pCacheConn->cmd("ZINCRBY", MEMBER_GROUP_UNREAD_KEY_PREFIX + to_string(toUserId), &argv1, nullptr);
//                    list<string> argv;
//                    argv.push_back(to_string(msg->sent_at()));
//                    argv.push_back(to_string(msg->group_id()));
//                    pCacheConn->cmd("ZADD", MEMBER_GROUP_UPDATE_KEY_PREFIX + to_string(toUserId), &argv, nullptr);
//
                }
            }
        }
        break;
    }

    if (pCacheConn) {
        pCacheManager->RelCacheConn(pCacheConn);
    }
}

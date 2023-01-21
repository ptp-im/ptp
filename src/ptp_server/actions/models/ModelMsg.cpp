/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.cpp
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：
 *
 *
================================================================*/
#include "ModelMsg.h"
#include "CachePool.h"

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

uint32_t CModelMsg::genMsgId(CacheConn *pCacheConnMsg,uint32_t group_id) {
    auto msg_id = pCacheConnMsg->incrBy(GROUP_MSG_ID_BY_ID_INCR_PREFIX + to_string(group_id), 1);
    return (uint32_t) msg_id;
}

void CModelMsg::saveMsgInfo(CacheConn *pCacheConnMsg, PTP::Common::MsgInfo *msg_info) {
    string msg_serialize = msg_info->SerializeAsString();
    string msg_info_string = bytes_to_hex_string((unsigned char *)msg_serialize.data(), msg_serialize.size());
    auto group_id_str = to_string(msg_info->group_id());
    auto msg_id_str = to_string(msg_info->msg_id());
    list<string> argv_set;
    argv_set.emplace_back("MSET");
    argv_set.push_back(GROUP_MSG_DATA_BY_ID_PREFIX + group_id_str + "_" + msg_id_str);
    argv_set.push_back(msg_info_string);
    argv_set.push_back(GROUP_LATEST_MSG_DATA_BY_ID_PREFIX + group_id_str);
    argv_set.push_back(msg_id_str);
    argv_set.push_back(GROUP_MSG_UPDATE_PREFIX + group_id_str);
    argv_set.push_back(to_string(time(nullptr)));
    pCacheConnMsg->exec(&argv_set, nullptr);
}

void CModelMsg::updateUnReadMsg(CacheConn *pCacheConnMsg,uint32_t groupId,uint32_t user_id,bool reset){
    list<string> argv;
    if(reset){
        argv.emplace_back("ZADD");
        argv.emplace_back(MEMBER_GROUP_UNREAD_KEY_PREFIX+ to_string(user_id));
        argv.emplace_back("0");
        argv.emplace_back(to_string(groupId));
        pCacheConnMsg->exec(&argv, nullptr);
    }else{
        argv.emplace_back("1");
        argv.push_back(to_string(groupId));
        pCacheConnMsg->cmd("ZINCRBY", MEMBER_GROUP_UNREAD_KEY_PREFIX + to_string(user_id), &argv, nullptr);
    }
}

void CModelMsg::getMsgListById(CacheConn *pCacheConnMsg,uint32_t groupId,list<uint32_t> &msg_ids,list<string> &msg_str_list){
    string group_id_str = to_string(groupId);
    list<string> argv_msg_list;
    list<string> ret_msg_list;
    argv_msg_list.emplace_back("MGET");
    for(uint32_t msg_id : msg_ids){
        argv_msg_list.push_back(GROUP_MSG_DATA_BY_ID_PREFIX + group_id_str + "_" + to_string(msg_id));
    }

    pCacheConnMsg->exec(&argv_msg_list,&ret_msg_list);
    for(string &it : ret_msg_list){
        string msg_hex = it.c_str();
        if(!msg_hex.empty()){
            msg_str_list.push_back(msg_hex);
        }
    }
}

void CModelMsg::getUnReadMsgList(CacheConn *pCacheConnMsg,uint32_t user_id,uint32_t &total,list<string> &unread_msg_list){
    list<string> argv;
    list<string> list_ret;
    argv.emplace_back("ZRANGE");
    argv.push_back(MEMBER_GROUP_UNREAD_KEY_PREFIX+to_string(user_id));
    argv.emplace_back("1");
    argv.emplace_back("+INF");
    argv.emplace_back("BYSCORE");
    argv.emplace_back("WITHSCORES");
    pCacheConnMsg->exec(&argv,&list_ret);

    list<string> argv_group_msg;
    argv_group_msg.emplace_back("MGET");
    for(auto it=list_ret.begin();it != list_ret.end();it++){
        advance(it,0);
        string group_id_str = *it;
        advance(it,1);
        uint32_t unread_cnt = string_to_int(*it);
        total += unread_cnt;
        argv_group_msg.push_back(GROUP_LATEST_MSG_DATA_BY_ID_PREFIX+group_id_str);
    }
    pCacheConnMsg->exec(&argv_group_msg,&unread_msg_list);
}


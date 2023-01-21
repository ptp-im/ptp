/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_MSG_H_
#define MODEL_MSG_H_

#include "PTP.Msg.pb.h"
#include "../../CachePool.h"

using namespace std;
using namespace PTP::Common;

class CModelMsg {
public:
virtual ~CModelMsg();
    static CModelMsg* getInstance();
    static uint32_t genMsgId(CacheConn *pCacheConnMsg,uint32_t group_id);
    static void saveMsgInfo(CacheConn *pCacheConnMsg,PTP::Common::MsgInfo* msg_info);
    static void updateUnReadMsg(CacheConn *pCacheConnMsg,uint32_t groupId,uint32_t user_id,bool reset);
    static void getMsgListById(CacheConn *pCacheConnMsg,uint32_t groupId,list<uint32_t> &msg_ids,list<string> &msg_str_list);
    static void getUnReadMsgList(CacheConn *pCacheConnMsg,uint32_t user_id,uint32_t &total,list<string> &unread_msg_list);
private:
    CModelMsg();
private:
    static CModelMsg*    m_pInstance;
};

#endif /* MODEL_MSG_H_ */

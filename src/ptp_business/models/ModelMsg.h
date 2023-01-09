/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Tuesday, November 29, 2022
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_MSG_H_
#define MODEL_MSG_H_

#include "PTP.Msg.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelMsg {
public:
virtual ~CModelMsg();

    static CModelMsg* getInstance();
    void MsgGetByIdsReq(PTP::Msg::MsgGetByIdsReq* msg, PTP::Msg::MsgGetByIdsRes* msg_rsp, ERR& error);
    void MsgGetMaxIdReq(PTP::Msg::MsgGetMaxIdReq* msg, PTP::Msg::MsgGetMaxIdRes* msg_rsp, ERR& error);
    void MsgReadAckReq(PTP::Msg::MsgReadAckReq* msg, PTP::Msg::MsgReadNotify* msg_rsp, ERR& error);
    void MsgReq(PTP::Msg::MsgReq* msg,PTP::Msg::MsgRes* msg_rsp,PTP::Msg::MsgNotify* msg_notify, ERR& error);
    void MsgUnNotify(PTP::Msg::MsgUnNotify* msg);
private:
    CModelMsg();
    
private:
    static CModelMsg*    m_pInstance;
};

#endif /* MODEL_MSG_H_ */

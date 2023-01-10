/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageContent.h
 *   创 建 者：PTP
 *   邮    箱：crypto.service@proton.me
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef MESSAGECOUTENT_H_
#define MESSAGECOUTENT_H_

#include "ImPduBase.h"

namespace DB_PROXY {

    void MsgReqProxy(CImPdu* pPdu, uint32_t conn_uuid);

    void MsgReadAckReqProxy(CImPdu* pPdu, uint32_t conn_uuid);

    void MsgGetMaxIdReqProxy(CImPdu* pPdu, uint32_t conn_uuid);

    void MsgGetByIdsReqProxy(CImPdu* pPdu, uint32_t conn_uuid);

    void MsgUnNotifyProxy(CImPdu* pPdu, uint32_t conn_uuid);


    void getMessage(CImPdu* pPdu, uint32_t conn_uuid);

    void sendMessage(CImPdu* pPdu, uint32_t conn_uuid);
    
    void getMessageById(CImPdu* pPdu, uint32_t conn_uuid);
    
    void getLatestMsgId(CImPdu* pPdu, uint32_t conn_uuid);
};

#endif /* MESSAGECOUTENT_H_ */
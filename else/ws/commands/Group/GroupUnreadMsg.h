/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupUnreadMsg.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPUNREADMSG_H__
#define __GROUPUNREADMSG_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {

    void GroupUnreadMsgReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupUnreadMsgResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPUNREADMSG_H__) */

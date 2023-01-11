/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  Msg.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSG_H__
#define __MSG_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {

    void MsgReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void MsgResCmd(CImPdu* pPdu);
};

#endif /*defined(__MSG_H__) */

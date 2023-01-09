/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadAck.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGREADACK_H__
#define __MSGREADACK_H__

#include "ImPduBase.h"

namespace COMMAND {

    void MsgReadAckReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__MSGREADACK_H__) */

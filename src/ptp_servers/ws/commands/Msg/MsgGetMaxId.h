/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetMaxId.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETMAXID_H__
#define __MSGGETMAXID_H__

#include "ImPduBase.h"

namespace COMMAND {

    void MsgGetMaxIdReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void MsgGetMaxIdResCmd(CImPdu* pPdu);
};

#endif /*defined(__MSGGETMAXID_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetALL.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETALL_H__
#define __BUDDYGETALL_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {

    void BuddyGetALLReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyGetALLResCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYGETALL_H__) */

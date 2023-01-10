/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetList.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETLIST_H__
#define __BUDDYGETLIST_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void BuddyGetListReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyGetListResCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYGETLIST_H__) */
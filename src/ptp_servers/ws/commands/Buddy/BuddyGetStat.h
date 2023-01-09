/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyGetStat.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYGETSTAT_H__
#define __BUDDYGETSTAT_H__

#include "ImPduBase.h"

namespace COMMAND {

    void BuddyGetStatReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyGetStatResCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYGETSTAT_H__) */

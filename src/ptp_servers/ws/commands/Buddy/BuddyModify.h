/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  BuddyModify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __BUDDYMODIFY_H__
#define __BUDDYMODIFY_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void BuddyModifyReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void BuddyModifyResCmd(CImPdu* pPdu);
};

#endif /*defined(__BUDDYMODIFY_H__) */

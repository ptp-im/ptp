/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetByIds.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETBYIDS_H__
#define __MSGGETBYIDS_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void MsgGetByIdsReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void MsgGetByIdsResCmd(CImPdu* pPdu);
};

#endif /*defined(__MSGGETBYIDS_H__) */

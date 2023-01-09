/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPMODIFY_H__
#define __GROUPMODIFY_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void GroupModifyReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupModifyResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPMODIFY_H__) */

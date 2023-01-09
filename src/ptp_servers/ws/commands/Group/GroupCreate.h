/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupCreate.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPCREATE_H__
#define __GROUPCREATE_H__

#include "ImPduBase.h"

namespace COMMAND {

    void GroupCreateReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupCreateResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPCREATE_H__) */

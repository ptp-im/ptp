/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupGetList.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPGETLIST_H__
#define __GROUPGETLIST_H__

#include "ImPduBase.h"

namespace COMMAND {

    void GroupGetListReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupGetListResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPGETLIST_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupPreCreate.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPPRECREATE_H__
#define __GROUPPRECREATE_H__

#include "ImPduBase.h"

namespace COMMAND {

    void GroupPreCreateReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void GroupPreCreateResCmd(CImPdu* pPdu);
};

#endif /*defined(__GROUPPRECREATE_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLogin.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __AUTHLOGIN_H__
#define __AUTHLOGIN_H__

#include "ImPduBase.h"

namespace COMMAND {

    void AuthLoginReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__AUTHLOGIN_H__) */

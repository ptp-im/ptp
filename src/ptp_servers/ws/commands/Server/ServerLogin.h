/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ServerLogin.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SERVERLOGIN_H__
#define __SERVERLOGIN_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void ServerLoginReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    void ServerLoginResCmd(CImPdu* pPdu);
};

#endif /*defined(__SERVERLOGIN_H__) */

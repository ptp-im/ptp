/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLogout.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __AUTHLOGOUT_H__
#define __AUTHLOGOUT_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void AuthLogoutReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__AUTHLOGOUT_H__) */

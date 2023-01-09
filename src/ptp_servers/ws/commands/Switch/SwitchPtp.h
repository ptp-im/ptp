/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtp.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SWITCHPTP_H__
#define __SWITCHPTP_H__

#include "ImPduBase.h"

namespace COMMAND {

    void SwitchPtpReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__SWITCHPTP_H__) */

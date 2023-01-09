/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthCaptcha.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __AUTHCAPTCHA_H__
#define __AUTHCAPTCHA_H__

#include "ImPduBase.h"

namespace COMMAND {

    void AuthCaptchaReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__AUTHCAPTCHA_H__) */

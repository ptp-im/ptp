/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  Captcha.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __CAPTCHA_H__
#define __CAPTCHA_H__

#include "ImPduBase.h"

namespace COMMAND {

    void CaptchaReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__CAPTCHA_H__) */

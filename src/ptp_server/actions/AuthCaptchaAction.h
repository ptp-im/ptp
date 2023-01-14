/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthCaptchaAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __AUTHCAPTCHAACTION_H__
#define __AUTHCAPTCHAACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_AUTH {
    void AuthCaptchaReqAction(CReuest *request, CResponse *response);
};

#endif /*defined(__AUTHCAPTCHAACTION_H__) */

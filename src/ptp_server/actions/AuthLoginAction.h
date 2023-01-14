/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  AuthLoginAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __AUTHLOGINACTION_H__
#define __AUTHLOGINACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_AUTH {
    void AuthLoginReqAction(CReuest *request, CResponse *response);
};

#endif /*defined(__AUTHLOGINACTION_H__) */

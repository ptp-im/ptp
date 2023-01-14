/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ServerLoginAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SERVERLOGINACTION_H__
#define __SERVERLOGINACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_SERVER {
    void ServerLoginReqAction(CRequest *request, CResponse *response);
    void ServerLoginResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__SERVERLOGINACTION_H__) */

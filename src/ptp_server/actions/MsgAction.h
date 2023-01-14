/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGACTION_H__
#define __MSGACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgReqAction(CRequest *request, CResponse *response);
    void MsgResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGACTION_H__) */

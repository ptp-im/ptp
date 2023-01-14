/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetMaxIdAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETMAXIDACTION_H__
#define __MSGGETMAXIDACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgGetMaxIdReqAction(CRequest *request, CResponse *response);
    void MsgGetMaxIdResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGGETMAXIDACTION_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetMaxIdAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETMAXIDACTION_H__
#define __MSGGETMAXIDACTION_H__

#include "../Request.h"

namespace ACTION_MSG {
    void MsgGetMaxIdReqAction(CRequest *request);
    void MsgGetMaxIdResAction(CRequest* request);
};

#endif /*defined(__MSGGETMAXIDACTION_H__) */

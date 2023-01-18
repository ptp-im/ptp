/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGACTION_H__
#define __MSGACTION_H__

#include "../Request.h"

namespace ACTION_MSG {
    void MsgReqAction(CRequest *request);
    void MsgResAction(CRequest* request);
};

#endif /*defined(__MSGACTION_H__) */

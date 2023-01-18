/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetByIdsAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETBYIDSACTION_H__
#define __MSGGETBYIDSACTION_H__

#include "../Request.h"

namespace ACTION_MSG {
    void MsgGetByIdsReqAction(CRequest *request);
    void MsgGetByIdsResAction(CRequest* request);
};

#endif /*defined(__MSGGETBYIDSACTION_H__) */

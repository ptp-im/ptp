/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgGetByIdsAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __MSGGETBYIDSACTION_H__
#define __MSGGETBYIDSACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgGetByIdsReqAction(CRequest *request, CResponse *response);
    void MsgGetByIdsResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGGETBYIDSACTION_H__) */

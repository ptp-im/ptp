/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGNOTIFYACTION_H__
#define __MSGNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGNOTIFYACTION_H__) */

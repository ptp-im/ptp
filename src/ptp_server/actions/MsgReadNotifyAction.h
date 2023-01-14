/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGREADNOTIFYACTION_H__
#define __MSGREADNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgReadNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGREADNOTIFYACTION_H__) */

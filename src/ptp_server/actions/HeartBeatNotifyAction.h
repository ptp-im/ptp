/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  HeartBeatNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __HEARTBEATNOTIFYACTION_H__
#define __HEARTBEATNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_OTHER {
    void HeartBeatNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__HEARTBEATNOTIFYACTION_H__) */

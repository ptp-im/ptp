/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgUnNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGUNNOTIFYACTION_H__
#define __MSGUNNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_MSG {
    void MsgUnNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__MSGUNNOTIFYACTION_H__) */

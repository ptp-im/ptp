/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPMODIFYNOTIFYACTION_H__
#define __GROUPMODIFYNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_GROUP {
    void GroupModifyNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPMODIFYNOTIFYACTION_H__) */

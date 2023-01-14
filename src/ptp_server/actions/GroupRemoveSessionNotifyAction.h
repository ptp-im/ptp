/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupRemoveSessionNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __GROUPREMOVESESSIONNOTIFYACTION_H__
#define __GROUPREMOVESESSIONNOTIFYACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupRemoveSessionNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPREMOVESESSIONNOTIFYACTION_H__) */

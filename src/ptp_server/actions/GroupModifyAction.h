/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPMODIFYACTION_H__
#define __GROUPMODIFYACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupModifyReqAction(CRequest *request);
    void GroupModifyResAction(CRequest* request);
};

#endif /*defined(__GROUPMODIFYACTION_H__) */

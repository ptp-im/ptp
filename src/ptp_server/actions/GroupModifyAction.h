/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupModifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPMODIFYACTION_H__
#define __GROUPMODIFYACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_GROUP {
    void GroupModifyReqAction(CReuest *request, CResponse *response);
    void GroupModifyResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPMODIFYACTION_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupPreCreateAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPPRECREATEACTION_H__
#define __GROUPPRECREATEACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_GROUP {
    void GroupPreCreateReqAction(CRequest *request, CResponse *response);
    void GroupPreCreateResAction(CRequest* request, CResponse *response);
};

#endif /*defined(__GROUPPRECREATEACTION_H__) */

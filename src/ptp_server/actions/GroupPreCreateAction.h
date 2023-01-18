/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  GroupPreCreateAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __GROUPPRECREATEACTION_H__
#define __GROUPPRECREATEACTION_H__

#include "../Request.h"

namespace ACTION_GROUP {
    void GroupPreCreateReqAction(CRequest *request);
    void GroupPreCreateResAction(CRequest* request);
};

#endif /*defined(__GROUPPRECREATEACTION_H__) */

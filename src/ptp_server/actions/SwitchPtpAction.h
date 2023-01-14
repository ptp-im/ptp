/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SWITCHPTPACTION_H__
#define __SWITCHPTPACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_SWITCH {
    void SwitchPtpReqAction(CRequest *request, CResponse *response);
};

#endif /*defined(__SWITCHPTPACTION_H__) */

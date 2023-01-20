/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SWITCHPTPACTION_H__
#define __SWITCHPTPACTION_H__

#include "../Request.h"

namespace ACTION_SWITCH {
    void SwitchPtpReqAction(CRequest *request);
    void SwitchPtpResAction(CRequest *request);
};

#endif /*defined(__SWITCHPTPACTION_H__) */

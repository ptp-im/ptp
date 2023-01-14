/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __SWITCHPTPNOTIFYACTION_H__
#define __SWITCHPTPNOTIFYACTION_H__

#include "Request.h"
#include "Response.h"

namespace ACTION_SWITCH {
    void SwitchPtpNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__SWITCHPTPNOTIFYACTION_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesNotifyAction.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Sunday, January 15, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __SWITCHDEVICESNOTIFYACTION_H__
#define __SWITCHDEVICESNOTIFYACTION_H__

#include "../Request.h"
#include "../Response.h"

namespace ACTION_SWITCH {
    void SwitchDevicesNotifyAction(CRequest* request, CResponse *response);
};

#endif /*defined(__SWITCHDEVICESNOTIFYACTION_H__) */

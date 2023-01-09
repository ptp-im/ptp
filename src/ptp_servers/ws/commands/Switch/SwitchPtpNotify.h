/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchPtpNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __SWITCHPTPNOTIFY_H__
#define __SWITCHPTPNOTIFY_H__

#include "ImPduBase.h"

namespace COMMAND {
    void SwitchPtpNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__SWITCHPTPNOTIFY_H__) */

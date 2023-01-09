/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevicesNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __SWITCHDEVICESNOTIFY_H__
#define __SWITCHDEVICESNOTIFY_H__

#include "ImPduBase.h"

namespace COMMAND {
    void SwitchDevicesNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__SWITCHDEVICESNOTIFY_H__) */

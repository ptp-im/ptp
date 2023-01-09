/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  HeartBeatNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __HEARTBEATNOTIFY_H__
#define __HEARTBEATNOTIFY_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {
    void HeartBeatNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__HEARTBEATNOTIFY_H__) */

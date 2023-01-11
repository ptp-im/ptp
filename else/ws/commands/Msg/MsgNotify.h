/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGNOTIFY_H__
#define __MSGNOTIFY_H__

#include "ptp_protobuf/ImPdu.h"

namespace COMMAND {
    void MsgNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__MSGNOTIFY_H__) */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  MsgReadNotify.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： 
 *
 #pragma once
================================================================*/
#ifndef __MSGREADNOTIFY_H__
#define __MSGREADNOTIFY_H__

#include "ImPduBase.h"

namespace COMMAND {
    void MsgReadNotifyCmd(CImPdu* pPdu);
};

#endif /*defined(__MSGREADNOTIFY_H__) */

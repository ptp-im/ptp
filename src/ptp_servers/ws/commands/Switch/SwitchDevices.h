/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  SwitchDevices.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 2, 2023
 *   desc： DO NOT EDIT!!
 *
 #pragma once
================================================================*/
#ifndef __SWITCHDEVICES_H__
#define __SWITCHDEVICES_H__

#include "ptp_global/ImPduBase.h"

namespace COMMAND {

    void SwitchDevicesReqCmd(CImPdu* pPdu, uint32_t conn_uuid);
    
};

#endif /*defined(__SWITCHDEVICES_H__) */

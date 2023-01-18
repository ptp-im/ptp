/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelSwitch.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_SWITCH_H_
#define MODEL_SWITCH_H_

#include "PTP.Switch.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelSwitch {
public:
virtual ~CModelSwitch();
    static CModelSwitch* getInstance();
private:
    CModelSwitch();
private:
    static CModelSwitch*    m_pInstance;
};

#endif /* MODEL_SWITCH_H_ */

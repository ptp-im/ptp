/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelMsg.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_MSG_H_
#define MODEL_MSG_H_

#include "PTP.Msg.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelMsg {
public:
virtual ~CModelMsg();
    static CModelMsg* getInstance();
private:
    CModelMsg();
private:
    static CModelMsg*    m_pInstance;
};

#endif /* MODEL_MSG_H_ */

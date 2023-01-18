/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelAuth.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_AUTH_H_
#define MODEL_AUTH_H_

#include "PTP.Auth.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelAuth {
public:
virtual ~CModelAuth();
    static CModelAuth* getInstance();
private:
    CModelAuth();
private:
    static CModelAuth*    m_pInstance;
};

#endif /* MODEL_AUTH_H_ */

/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelOther.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_OTHER_H_
#define MODEL_OTHER_H_

#include "PTP.Other.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelOther {
public:
virtual ~CModelOther();
    static CModelOther* getInstance();
private:
    CModelOther();
private:
    static CModelOther*    m_pInstance;
};

#endif /* MODEL_OTHER_H_ */

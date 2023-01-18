/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelServer.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Monday, January 16, 2023
 *   desc：DO NOT EDIT!!
 *
 *
 ================================================================*/

#ifndef MODEL_SERVER_H_
#define MODEL_SERVER_H_

#include "PTP.Server.pb.h"

using namespace std;
using namespace PTP::Common;

class CModelServer {
public:
virtual ~CModelServer();
    static CModelServer* getInstance();
private:
    CModelServer();
private:
    static CModelServer*    m_pInstance;
};

#endif /* MODEL_SERVER_H_ */

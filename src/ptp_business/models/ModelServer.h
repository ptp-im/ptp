/*================================================================
 *   Copyright (C) 2022 All rights reserved.
 *
 *   filename：  ModelServer.h
 *   author：    Barry
 *   email：     dev.crypto@proton.me
 *   createdAt： Tuesday, November 29, 2022
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
    void ServerLoginReq(PTP::Server::ServerLoginReq* msg, PTP::Server::ServerLoginRes* msg_rsp, ERR& error);
private:
    CModelServer();
    
private:
    static CModelServer*    m_pInstance;
};

#endif /* MODEL_SERVER_H_ */

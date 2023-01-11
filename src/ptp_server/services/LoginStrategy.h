#ifndef __LOGINSTRATEGY_H__
#define __LOGINSTRATEGY_H__

#include <iostream>

#include "PTP.Server.pb.h"
#include "PTP.Common.pb.h"

class CLoginStrategy
{
public:
    bool ServerLogin(PTP::Server::ServerLoginReq *msg, PTP::Common::UserInfo* user,PTP::Common::ERR& error);
};

#endif /*defined(__LOGINSTRATEGY_H__) */

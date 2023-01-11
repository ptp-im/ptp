#ifndef __INTERLOGIN_H__
#define __INTERLOGIN_H__

#include "LoginStrategy.h"

class CInterLoginStrategy :public CLoginStrategy
{
public:
    virtual bool ServerLogin(PTP::Server::ServerLoginReq *msg, PTP::Common::UserInfo* user,PTP::Common::ERR& error);
};

#endif /*defined(__INTERLOGIN_H__) */

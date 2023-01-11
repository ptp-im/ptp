#ifndef __INTERLOGIN_H__
#define __INTERLOGIN_H__

#include "LoginStrategy.h"

class CInterLoginStrategy :public CLoginStrategy
{
public:
    virtual bool ServerLogin(const std::string sign, const std::string& captcha, const std::string &address, PTP::Common::UserInfo* user,PTP::Common::ERR& error);
};

#endif /*defined(__INTERLOGIN_H__) */

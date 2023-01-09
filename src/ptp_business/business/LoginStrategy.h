#ifndef __LOGINSTRATEGY_H__
#define __LOGINSTRATEGY_H__

#include <iostream>

#include "IM.BaseDefine.pb.h"

#include "PTP.Server.pb.h"
#include "PTP.Common.pb.h"

class CLoginStrategy
{
public:
    virtual bool doLogin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user) = 0;
    virtual bool ServerLogin(const std::string sign, const std::string& captcha, const std::string &address, PTP::Common::UserInfo* user,PTP::Common::ERR& error) = 0;
};

#endif /*defined(__LOGINSTRATEGY_H__) */

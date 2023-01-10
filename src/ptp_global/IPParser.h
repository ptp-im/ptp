#ifndef _IPPARSER_H_
#define _IPPARSER_H_

#include "ptp_global/Utils.h"

class IpParser
{
    public:
        IpParser();
        virtual ~IpParser();
    bool isTelcome(const char* ip);
};

#endif


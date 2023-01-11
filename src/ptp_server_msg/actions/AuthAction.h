
#ifndef __MSG_ACTIONS_AUTH_H__
#define __MSG_ACTIONS_AUTH_H__
#include "../Request.h"
#include "../Response.h"

namespace ACTION {
    void ServerLoginReq(CRequest* request, CResponse* response);
    void ServerLoginRes(CRequest* request, CResponse* response);
}

#endif /*defined(__MSG_ACTIONS_AUTH_H__) */

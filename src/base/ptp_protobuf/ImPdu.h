#ifndef __IMPDU_H__
#define __IMPDU_H__

#include "ptp_global/UtilPdu.h"
#include "ptp_global/global.h"
#include "ptp_global/ImPduBase.h"
#include "ptp_global/Utils.h"
#include "ptp_global/Logger.h"
#include "PTP.Common.pb.h"
#include "ActionCommands.h"

using namespace PTP::Common;

class DLL_MODIFIER ImPdu: public CImPdu
{
public:
    ImPdu();
    virtual ~ImPdu() {};
    void SetPBMsg(unsigned char *buf, int len);
    void SetPBMsg(const google::protobuf::MessageLite* msg);
    void SetPBMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num = 0);
    void Dump();
};


#endif /* __IMPDU_H__ */

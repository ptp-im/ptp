#ifndef PTP_IM_CONN_UTIL_H
#define PTP_IM_CONN_UTIL_H

#include "global.h"

namespace google { namespace protobuf {
        class MessageLite;
    }}

class CImConn;

int SendMessageLite(CImConn* conn, uint16_t sid, uint16_t cid, const ::google::protobuf::MessageLite* message);
int SendMessageLite(CImConn* conn, uint16_t sid, uint16_t cid, uint16_t seq_num, const ::google::protobuf::MessageLite* message);
int SendMessageLite(CImConn* conn, uint16_t sid, uint16_t cid, uint16_t seq_num, uint16_t error, const ::google::protobuf::MessageLite* message);

#endif /* defined(PTP_IM_CONN_UTIL_H) */

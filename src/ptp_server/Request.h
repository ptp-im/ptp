//
// Created by jack on 2023/1/11.
//

#ifndef PTP_REQUEST_H
#define PTP_REQUEST_H
#include "ImPdu.h"
#include "ptp_global/Utils.h"

class CRequest{
public:
    CRequest();
    ~CRequest();
    void SetRequestPdu(ImPdu *pPdu);
    ImPdu *GetRequestPdu(){return m_request_pPdu;};
    ImPdu *GetResponsePdu(){return m_response_pPdu;};
    uint16_t GetSeqNum(){return m_seq_num;};
    void SetIsBusinessConn(bool isBusinessConn);
    bool IsBusinessConn() const{return m_isBusinessConn;};
    net_handle_t GetHandle() const{return m_handle;}
    void SetHandle(net_handle_t handle){m_handle = handle;}
    bool IsResponsePduValid();
    bool IsNext(){return m_next;};
    void SendResponsePdu(ImPdu *pPdu,bool encrypt = false);
    void SendResponseMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num = 0,bool encrypt = false);
    void Next(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num = 0);
private:
    ImPdu           *m_request_pPdu;
    ImPdu           *m_response_pPdu;
    bool            m_next;
    uint16_t        m_seq_num;
    net_handle_t    m_handle;
    bool            m_isBusinessConn;
};


#endif //PTP_REQUEST_H

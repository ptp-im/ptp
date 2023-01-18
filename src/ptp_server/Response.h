//
// Created by jack on 2023/1/11.
//

#ifndef PTP_RESPONSE_H
#define PTP_RESPONSE_H
#include "ptp_global/Utils.h"

#include "ImPdu.h"

class CResponse: public CRefObject {
public:
    CResponse();
    ~CResponse();
    void SetPdu(ImPdu *pPdu){m_pPdu->Write(pPdu->GetBuffer(),pPdu->GetLength());}
    void SetBroadcast(bool broadcast){m_broadcast = broadcast;}
    ImPdu *GetPdu(){return m_pPdu;}
    bool isPduValid();
    bool isNext(){return m_next;};
    void SendPdu(ImPdu *pPdu,bool encrypt = false);
    void SendMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num = 0,bool encrypt = false);
    void Next(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num = 0);
private:
    ImPdu  *m_pPdu;
    bool   m_broadcast;
    bool   m_next;
};


#endif //PTP_RESPONSE_H

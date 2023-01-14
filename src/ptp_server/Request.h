//
// Created by jack on 2023/1/11.
//

#ifndef PTP_REQUEST_H
#define PTP_REQUEST_H
#include "ImPdu.h"
#include "ptp_global/Utils.h"

class CRequest: public CRefObject{
public:
    CRequest();
    ~CRequest();
    void SetPdu(ImPdu *pPdu){m_pPdu = pPdu;};
    ImPdu *GetPdu(){return m_pPdu;};
    void SetIsBusinessConn(bool isBusinessConn);
    bool IsBusinessConn(){return m_isBusinessConn;};
    net_handle_t GetHandle(){return m_handle;}
    void SetHandle(net_handle_t handle){m_handle = handle;}
private:
    ImPdu           *m_pPdu;
    net_handle_t    m_handle;
    bool            m_isBusinessConn;
};


#endif //PTP_REQUEST_H

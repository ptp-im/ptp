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
    ImPdu *GetPdu(){return m_pPdu;}
private:
    ImPdu  *m_pPdu;
};


#endif //PTP_REQUEST_H

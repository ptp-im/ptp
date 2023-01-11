//
// Created by jack on 2023/1/11.
//

#include "Request.h"

CRequest::CRequest() {
    m_pPdu = new ImPdu();
}

CRequest::~CRequest() {
    if(m_pPdu != NULL){
        m_pPdu = NULL;
    }
}
//
//void CRequest::SetPdu(ImPdu *pPdu) {
//    auto pdu_len = CByteStream::ReadUint32(pPdu->GetBuffer());
//    m_pPdu = (ImPdu*)ImPdu::ReadPdu(pPdu->GetBuffer(),pPdu->GetLength());
//}

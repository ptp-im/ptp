//
// Created by jack on 2023/1/11.
//

#include "Response.h"

CResponse::CResponse() {
    m_broadcast = false;
    m_next = false;
    m_pPdu = NULL;
}

CResponse::~CResponse() {
    if(m_pPdu != NULL){
        m_pPdu = NULL;
    }
}

bool CResponse::isPduValid(){
    if(m_pPdu == nullptr || m_pPdu->GetBuffer() == nullptr){
        return false;
    }
    auto pdu_len = CByteStream::ReadUint32(m_pPdu->GetBuffer());
    return pdu_len <= m_pPdu->GetLength();
}


void CResponse::Next(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num) {
    m_pPdu = new ImPdu();
    m_next = true;
    m_pPdu->SetPBMsg(
            msg,
            command_id,
            seq_num);
}

void CResponse::SendPdu(ImPdu *pPdu,bool encrypt){
    m_pPdu = new ImPdu();
    SetPdu(pPdu);
    m_pPdu->SetReversed(encrypt ? 1 : 0);
}

void CResponse::SendMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num,bool encrypt) {
    m_pPdu = new ImPdu();
    m_pPdu->SetPBMsg(
            msg,
            command_id,
            seq_num);
    m_pPdu->SetReversed(encrypt ? 1 : 0);
}



//
// Created by jack on 2023/1/11.
//

#include "Request.h"

CRequest::CRequest() {
    m_seq_num = 0;
    m_next = false;
    m_isBusinessConn = false;
    m_response_pPdu = new ImPdu();
    m_request_pPdu = new ImPdu();
    m_response_next_pPdu= new ImPdu();
}

CRequest::~CRequest() {
    if(m_response_pPdu != NULL){
        m_response_pPdu = NULL;
    }
    if(m_request_pPdu != NULL){
        m_request_pPdu = NULL;
    }
    if(m_response_next_pPdu != NULL){
        m_response_next_pPdu = NULL;
    }
}

void CRequest::SetIsBusinessConn(bool isBusinessConn) {
    m_isBusinessConn = isBusinessConn;
}

bool CRequest::IsResponsePduValid(){
    if(m_response_pPdu == nullptr || m_response_pPdu->GetBuffer() == nullptr){
        return false;
    }
    auto pdu_len = CByteStream::ReadUint32(m_response_pPdu->GetBuffer());
    return pdu_len <= m_response_pPdu->GetLength();
}

bool CRequest::IsNextResponsePduValid(){
    if(m_response_next_pPdu == nullptr || m_response_next_pPdu->GetBuffer() == nullptr){
        return false;
    }
    auto pdu_len = CByteStream::ReadUint32(m_response_next_pPdu->GetBuffer());
    return pdu_len <= m_response_next_pPdu->GetLength();
}

void CRequest::Next(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num) {
    m_next = true;
    m_response_next_pPdu = new ImPdu();
    m_response_next_pPdu->SetPBMsg(
            msg,
            command_id,
            seq_num);
}

void CRequest::SendResponsePdu(ImPdu *pPdu,bool encrypt){
    m_response_pPdu = pPdu;
    m_response_pPdu->SetReversed(encrypt ? 1 : 0);
    m_response_pPdu->SetSeqNum(m_seq_num);
}

void CRequest::SendResponseMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num,bool encrypt) {
    m_response_pPdu = new ImPdu();
    m_response_pPdu->SetPBMsg(
            msg,
            command_id,
            seq_num);
    m_response_pPdu->SetReversed(encrypt ? 1 : 0);
}

void CRequest::SetRequestPdu(ImPdu *pPdu) {
    m_request_pPdu = pPdu;
    m_seq_num = pPdu->GetSeqNum();
}



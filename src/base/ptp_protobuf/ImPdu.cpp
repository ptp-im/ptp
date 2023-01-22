#include "ImPdu.h"

#include "ptp_global/Helpers.h"


ImPdu::ImPdu(){};

void ImPdu::SetPBMsg(const google::protobuf::MessageLite* msg)
{
    m_buf.Read(NULL, m_buf.GetWriteOffset());
    m_buf.Write(NULL, sizeof(PduHeader_t));
    uint32_t msg_size = msg->ByteSizeLong();
    auto* szData = new uchar_t[msg_size];
    ALLOC_FAIL_ASSERT(szData)
    if (!msg->SerializeToArray(szData, (int)msg_size))
    {
        DEBUG_E("pb msg miss required fields.");
        return;
    }
    m_buf.Write(szData, msg_size);
    delete []szData;
    WriteHeader();
}

void ImPdu::SetPBMsg(const google::protobuf::MessageLite* msg,uint16_t command_id,uint16_t seq_num)
{
    m_pdu_header.command_id = command_id;
    //m_pdu_header.service_id = 0;
    m_pdu_header.seq_num = seq_num;
    m_buf.Read(NULL, m_buf.GetWriteOffset());
    m_buf.Write(NULL, sizeof(PduHeader_t));
    uint32_t msg_size = msg->ByteSizeLong();
    auto* szData = new uchar_t[msg_size];
    ALLOC_FAIL_ASSERT(szData)
    if (!msg->SerializeToArray(szData, (int)msg_size))
    {
        DEBUG_E("pb msg miss required fields.");
        return;
    }
    m_buf.Write(szData, msg_size);
    delete []szData;
    m_buf.Position(msg_size + sizeof(PduHeader_t));
    WriteHeader();
}


void ImPdu::SetPBMsg(unsigned char *buf, int len)
{
    m_buf.Read(NULL, m_buf.GetWriteOffset());
    m_buf.Write(NULL, sizeof(PduHeader_t));
    uchar_t* szData = new uchar_t[len];
    memcpy(szData,buf,len);
    m_buf.Write(szData, len);
    delete []szData;
    WriteHeader();
}

void ImPdu::Dump(){
    DEBUG_D("Dump  =====>>>>cid=%s",getActionCommandsName((ActionCommands)GetCommandId()).c_str());
    DEBUG_D("sno=%d",GetSeqNum());
    DEBUG_D("revs=%d",GetReversed());
    DEBUG_D("size=%d",GetLength());
    DEBUG_D("body size=%d",GetBodyLength());
    DEBUG_D("body  : %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer()+16,GetLength() - 16).c_str());
    DEBUG_D("header: %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer(),16).c_str());
    DEBUG_D("pdu   : %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer(),GetLength()).c_str());
}
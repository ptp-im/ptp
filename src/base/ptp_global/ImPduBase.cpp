#include "ImPduBase.h"
#include "Helpers.h"

#ifdef ANDROID
#else
#endif

CImPdu::CImPdu()
{
	m_pdu_header.version = IM_PDU_VERSION;
	m_pdu_header.flag = 0;
	//m_pdu_header.service_id = 0;
	m_pdu_header.command_id = 0;
	m_pdu_header.seq_num = 0;
    m_pdu_header.reversed = 0;
}

uchar_t* CImPdu::GetBuffer()
{
    return m_buf.GetBuffer();
}

uint32_t CImPdu::GetLength()
{
    return m_buf.GetWriteOffset();
}

uchar_t* CImPdu::GetBodyData()
{
    return m_buf.GetBuffer() + sizeof(PduHeader_t);
}

uint32_t CImPdu::GetBodyLength()
{
    uint32_t body_length = 0;
    body_length = m_buf.GetWriteOffset() - sizeof(PduHeader_t);
    return body_length;
}

void CImPdu::WriteHeader()
{
	uchar_t* buf = GetBuffer();
	CByteStream::WriteInt32(buf, GetLength());
	CByteStream::WriteUint16(buf + 4, m_pdu_header.version);
	CByteStream::WriteUint16(buf + 6, m_pdu_header.flag);
	CByteStream::WriteUint16(buf + 8, m_pdu_header.command_id);
	CByteStream::WriteUint16(buf + 10, m_pdu_header.seq_num);
    CByteStream::WriteUint32(buf + 12, m_pdu_header.reversed);
    m_pdu_header.length = GetLength();
}

void CImPdu::SetVersion(uint16_t version)
{
    m_pdu_header.version = version;
	uchar_t* buf = GetBuffer();
	CByteStream::WriteUint16(buf + 4, version);
}

void CImPdu::SetFlag(uint16_t flag)
{
    m_pdu_header.flag = flag;
    uchar_t* buf = GetBuffer();
	CByteStream::WriteUint16(buf + 6, flag);
}

void CImPdu::SetServiceId(uint16_t service_id)
{
//    uchar_t* buf = GetBuffer();
//    CByteStream::WriteUint16(buf + 8, service_id);
}

void CImPdu::SetCommandId(uint16_t command_id)
{
    m_pdu_header.command_id = command_id;
    uchar_t* buf = GetBuffer();
    CByteStream::WriteUint16(buf + 8, command_id);
}

void CImPdu::SetError(uint16_t error)
{
    uchar_t* buf = GetBuffer();
    CByteStream::WriteUint16(buf + 8, error);
}

void CImPdu::SetSeqNum(uint16_t seq_num)
{
    m_pdu_header.seq_num = seq_num;
	uchar_t* buf = GetBuffer();
	CByteStream::WriteUint16(buf + 10, seq_num);
}

void CImPdu::SetReversed(uint32_t reversed)
{
    uchar_t* buf = GetBuffer();
    m_pdu_header.reversed = reversed;
    CByteStream::WriteUint32(buf + 12, reversed);
}

int CImPdu::ReadPduHeader(uchar_t* buf, uint32_t len)
{
	int ret = -1;
	if (len >= IM_PDU_HEADER_LEN && buf) {
		CByteStream is(buf, len);

		is >> m_pdu_header.length;
		is >> m_pdu_header.version;
		is >> m_pdu_header.flag;
		is >> m_pdu_header.command_id;
		is >> m_pdu_header.seq_num;
        is >> m_pdu_header.reversed;
		ret = 0;
	}
	return ret;
}

CImPdu* CImPdu::ReadPdu(uchar_t *buf, uint32_t len)
{
	uint32_t pdu_len = 0;
	if (!IsPduAvailable(buf, len, pdu_len))
		return NULL;
	CImPdu* pPdu = NULL;
    pPdu = new CImPdu();
    pPdu->Write(buf, pdu_len);
    pPdu->ReadPduHeader(buf, IM_PDU_HEADER_LEN);

    return pPdu;
}

bool CImPdu::IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len)
{
	if (len < IM_PDU_HEADER_LEN)
		return false;

	pdu_len = CByteStream::ReadUint32(buf);
	if (pdu_len > len)
	{
		DEBUG_D("pdu_len=%d, len=%d", pdu_len, len);
		return false;
	}

    if(0 == pdu_len)
    {
        throw CPduException(1, "pdu_len is 0");
    }

	return true;
}

void CImPdu::SetPBMsg(const google::protobuf::MessageLite* msg)
{
    if(msg != nullptr){
        DEBUG_E("SetPBMsg error please use CImPdu");
        return;
    }
    m_buf.Read(NULL, m_buf.GetWriteOffset());
    m_buf.Write(NULL, sizeof(PduHeader_t));
    uint32_t msg_size = 0;
    uchar_t* szData = new uchar_t[msg_size];
    m_buf.Write(szData, msg_size);
    delete []szData;
    WriteHeader();
}

void CImPdu::SetPBMsg(unsigned char *buf, int len)
{
    m_buf.Read(NULL, m_buf.GetWriteOffset());
    m_buf.Write(NULL, sizeof(PduHeader_t));
    uchar_t* szData = new uchar_t[len];
    memcpy(szData,buf,len);
    m_buf.Write(szData, len);
    delete []szData;
    WriteHeader();
}


void CImPdu::Dump(){
    DEBUG_D("PDU Dump  =====>>>>");
    DEBUG_D("cid     = %d",GetCommandId());
    DEBUG_D("sno     = %d",GetSeqNum());
    DEBUG_D("rev     = %d",GetReversed());
    DEBUG_D("len     = %d",GetLength());
    DEBUG_D("len b   = %d",GetBodyLength());
    DEBUG_D("header  : %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer(),16).c_str());
    DEBUG_D("pdu     : %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer(),32).c_str());
    DEBUG_D("body    : %s", bytes_to_hex_string((unsigned char *)m_buf.GetBuffer()+16,16).c_str());
}

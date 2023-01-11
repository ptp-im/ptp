#ifndef BusinessClientConn_H_
#define BusinessClientConn_H_

#include "ptp_global/ImConn.h"
#include "ptp_global/ServInfo.h"

class CBusinessClientConn : public CImConn
{
public:
	CBusinessClientConn();
	virtual ~CBusinessClientConn();

	bool IsOpen() { return m_bOpen; }

	void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx);
	virtual void Close();
	virtual void OnConfirm();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(CImPdu* pPdu);
    bool isEnable();
private:

private:
	bool 		m_bOpen;
	uint32_t	m_serv_idx;
};

void init_business_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt);

CBusinessClientConn* get_business_serv_conn_for_login();
CBusinessClientConn* get_business_serv_conn();

void set_enable_business_async(bool flag);

#endif /* BusinessClientConn_H_ */

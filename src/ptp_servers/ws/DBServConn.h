#ifndef DBSERVCONN_H_
#define DBSERVCONN_H_

#include "ptp_global/ImConn.h"
#include "ptp_global/ServInfo.h"
#include "RouteServConn.h"

class CDBServConn : public CImConn
{
public:
	CDBServConn();
	virtual ~CDBServConn();

	bool IsOpen() { return m_bOpen; }

	void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx);
	virtual void Close();

	virtual void OnConfirm();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(CImPdu* pPdu);
private:

private:
	bool 		m_bOpen;
	uint32_t	m_serv_idx;
};

void init_db_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt);

CDBServConn* get_db_serv_conn_for_login();
CDBServConn* get_db_serv_conn();

#endif /* DBSERVCONN_H_ */

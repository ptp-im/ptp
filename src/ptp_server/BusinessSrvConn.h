#ifndef BUSINESSSRVCONN_H_
#define BUSINESSSRVCONN_H_

#include <curl/curl.h>
#include "ptp_global/Utils.h"
#include "ptp_global/ImConn.h"
#include "HandlerMap.h"

typedef struct {
	uint32_t	conn_uuid;
	ImPdu*		pPdu;
} ResponsePdu_t;

class CBusinessSrvConn : public CImConn {
public:
	CBusinessSrvConn();
	virtual ~CBusinessSrvConn();

	virtual void Close();

	virtual void OnConnect(net_handle_t handle);
	virtual void OnRead();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	void HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len);

	static void AddResponsePdu(uint32_t conn_uuid, ImPdu* pPdu);	// 工作线程调用
	static void SendResponsePduList();	// 主线程调用
private:
	// 由于处理请求和发送回复在两个线程，socket的handle可能重用，所以需要用一个一直增加的uuid来表示一个连接
	static uint32_t	s_uuid_alloctor;
	uint32_t		m_uuid;

	static CLock			s_list_lock;
	static list<ResponsePdu_t*>	s_response_pdu_list;	// 主线程发送回复消息
};

int init_proxy_conn(uint32_t thread_num);
CBusinessSrvConn* get_proxy_conn_by_uuid(uint32_t uuid);
int run_ptp_server_business(int argc, char* argv[]);
#endif /* BUSINESSSRVCONN_H_ */

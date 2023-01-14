#ifndef __PROXY_TASK_H__
#define __PROXY_TASK_H__
#include "ptp_global/Task.h"
#include "ptp_global/Utils.h"
#include "ptp_global/ImPduBase.h"
#include "actions/HandlerMap.h"


class CProxyTask:public CTask
{
public:
    CProxyTask(uint32_t conn_uuid, pdu_handler_t pdu_handler, ImPdu* pPdu);
	virtual ~CProxyTask();
    
	virtual void run();
private:
	uint32_t 		m_conn_uuid;
    uint32_t 		m_pdu_reserved;
	pdu_handler_t	m_pdu_handler;
	ImPdu* 		m_pPdu;
};
#endif


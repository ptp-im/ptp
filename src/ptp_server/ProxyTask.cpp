#include "ProxyTask.h"
#include "BusinessSrvConn.h"
#include "Request.h"

CProxyTask::CProxyTask(uint32_t conn_uuid, pdu_handler_t pdu_handler, ImPdu* pPdu)
{
	m_conn_uuid = conn_uuid;
	m_pdu_handler = pdu_handler;
    m_pPdu = pPdu;
    m_pdu_reserved = m_pPdu ? m_pPdu->GetReversed() : 0;
}

CProxyTask::~CProxyTask()
{
    if (m_pPdu) {
		delete m_pPdu;
	}
}

void CProxyTask::run()
{
	if (!m_pPdu) {
		// tell CBusinessSrvConn to close connection with m_conn_uuid
        CBusinessSrvConn::AddResponsePdu(m_conn_uuid, NULL);
        DEBUG_E("CProxyTask::run,m_pPdu is null");
	} else {
        DEBUG_I("CProxyTask::run cid=%d,pid:%d", m_pPdu->GetCommandId(),getpid());
		if (m_pdu_handler) {
            CRequest request;
            request.SetIsBusinessConn(true);
            request.SetHandle((int)m_conn_uuid);
            request.SetRequestPdu(m_pPdu);
            m_pdu_handler(&request);
            if(request.IsResponsePduValid()){
//                if(response.isNext()){
//                    m_pPdu = response.GetPdu();
//                    m_pdu_handler = s_handler_map->GetHandler(pPdu->GetCommandId());
//                    if (handler) {
//                        CTask* pTask = new CProxyTask(m_uuid, handler, pPdu);
//                        g_thread_pool.AddTask(pTask);
//                    } else {
//                        DEBUG_I("no handler for packet type: %d", pPdu->GetCommandId());
//                    }
//                    run();
//                }else{
//                    auto pPdu = response.GetPdu();
//                    pPdu->SetReversed(m_pdu_reserved);
//                    CBusinessSrvConn::AddResponsePdu(m_conn_uuid, pPdu);
//                }
                auto pPdu = request.GetResponsePdu();
                pPdu->SetReversed(m_pdu_reserved);
                CBusinessSrvConn::AddResponsePdu(m_conn_uuid, pPdu);
            }else{
                CBusinessSrvConn::AddResponsePdu(m_conn_uuid, NULL);
                DEBUG_E("CProxyTask::run,response Pdu is inValid");
            }
        }
	}
}

#ifndef HANDLERMAP_H_
#define HANDLERMAP_H_

#include "ptp_global/Utils.h"
#include "ptp_protobuf/ImPdu.h"

typedef void (*pdu_handler_t)(CImPdu* pPdu, uint32_t conn_uuid);
typedef void (*pdu_handler_callbakc_t)(CImPdu* pPdu);

typedef map<uint32_t, pdu_handler_t> HandlerMap_t;
typedef map<uint32_t, pdu_handler_callbakc_t> HandlerCallbackMap_t;

class CHandlerMap {
public:
	virtual ~CHandlerMap();

	static CHandlerMap* getInstance();

	void Init();
	pdu_handler_t GetHandler(uint32_t pdu_type);
    pdu_handler_callbakc_t GetHandlerCallback(uint32_t pdu_type);
private:
	CHandlerMap();

private:
	static  CHandlerMap* s_handler_instance;

	HandlerMap_t 	m_handler_map;
    HandlerCallbackMap_t 	m_handler_callback_map;
};

#endif /* HANDLERMAP_H_ */

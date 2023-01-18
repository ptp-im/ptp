#ifndef HANDLERMAP_H_
#define HANDLERMAP_H_

#include "ptp_global/Utils.h"
#include "ptp_protobuf/ImPdu.h"
#include "../Request.h"

typedef void (*pdu_handler_t)(CRequest*);
typedef map<uint32_t, pdu_handler_t> HandlerMap_t;

class CHandlerMap {
public:
	virtual ~CHandlerMap();

	static CHandlerMap* getInstance();
	void Init();
	pdu_handler_t GetHandler(uint32_t pdu_cid);
private:
	CHandlerMap();

private:
	static  CHandlerMap* s_handler_instance;
	HandlerMap_t 	m_handler_map;
};

#endif /* HANDLERMAP_H_ */

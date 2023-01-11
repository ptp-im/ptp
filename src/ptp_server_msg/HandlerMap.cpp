#include "HandlerMap.h"

#include "actions/AuthAction.h"

using namespace PTP::Common;

void CHandlerMap::Init()
{
    m_handler_map.insert(make_pair(uint32_t(CID_ServerLoginReq), ACTION::ServerLoginReq));
    m_handler_map.insert(make_pair(uint32_t(CID_ServerLoginRes), ACTION::ServerLoginRes));
}

CHandlerMap* CHandlerMap::s_handler_instance = NULL;
/**
 *  构造函数
 */
CHandlerMap::CHandlerMap(){}

/**
 *  析构函数
 */
CHandlerMap::~CHandlerMap(){}

/**
 *  单例
 *
 *  @return 返回指向CHandlerMap的单例指针
 */
CHandlerMap* CHandlerMap::getInstance()
{
	if (!s_handler_instance) {
		s_handler_instance = new CHandlerMap();
	}
	return s_handler_instance;
}


/**
 *  通过commandId获取处理函数
 *
 *  @param pdu_cid commandId
 *
 *  @return 处理函数的函数指针
 */
pdu_handler_t CHandlerMap::GetHandler(uint32_t pdu_cid)
{
	auto it = m_handler_map.find(pdu_cid);
	if (it != m_handler_map.end()) {
		return it->second;
	} else {
		return NULL;
	}
}



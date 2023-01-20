#include "HandlerMap.h"
#include "ActionCommands.h"
#include "actions/AuthCaptchaAction.h"
#include "actions/AuthLoginAction.h"
#include "actions/AuthLogoutAction.h"
#include "actions/BuddyGetALLAction.h"
#include "actions/BuddyGetListAction.h"
#include "actions/BuddyGetStatAction.h"
#include "actions/BuddyImportContactsAction.h"
#include "actions/BuddyModifyNotifyAction.h"
#include "actions/BuddyModifyAction.h"
#include "actions/BuddyModifyUpdatePairNotifyAction.h"
#include "actions/BuddyQueryListAction.h"
#include "actions/BuddyStatNotifyAction.h"
#include "actions/FileImgDownloadAction.h"
#include "actions/FileImgUploadAction.h"
#include "actions/GroupChangeMemberNotifyAction.h"
#include "actions/GroupChangeMemberAction.h"
#include "actions/GroupCreateAction.h"
#include "actions/GroupGetListAction.h"
#include "actions/GroupGetMembersListAction.h"
#include "actions/GroupModifyNotifyAction.h"
#include "actions/GroupModifyAction.h"
#include "actions/GroupPreCreateAction.h"
#include "actions/GroupRemoveSessionNotifyAction.h"
#include "actions/GroupRemoveSessionAction.h"
#include "actions/GroupUnreadMsgAction.h"
#include "actions/MsgGetByIdsAction.h"
#include "actions/MsgGetMaxIdAction.h"
#include "actions/MsgNotifyAction.h"
#include "actions/MsgReadAckAction.h"
#include "actions/MsgReadNotifyAction.h"
#include "actions/MsgAction.h"
#include "actions/MsgUnNotifyAction.h"
#include "actions/CaptchaAction.h"
#include "actions/HeartBeatNotifyAction.h"
#include "actions/ServerLoginAction.h"
#include "actions/SwitchDevicesNotifyAction.h"
#include "actions/SwitchDevicesAction.h"
#include "actions/SwitchPtpNotifyAction.h"
#include "actions/SwitchPtpAction.h"
using namespace PTP::Common;

void CHandlerMap::Init()
{
    m_handler_map.insert(make_pair(uint32_t(CID_AuthCaptchaReq), ACTION_AUTH::AuthCaptchaReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_AuthLoginReq), ACTION_AUTH::AuthLoginReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_AuthLogoutReq), ACTION_AUTH::AuthLogoutReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetALLReq), ACTION_BUDDY::BuddyGetALLReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetALLRes), ACTION_BUDDY::BuddyGetALLResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetListReq), ACTION_BUDDY::BuddyGetListReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetListRes), ACTION_BUDDY::BuddyGetListResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetStatReq), ACTION_BUDDY::BuddyGetStatReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetStatRes), ACTION_BUDDY::BuddyGetStatResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyImportContactsReq), ACTION_BUDDY::BuddyImportContactsReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyImportContactsRes), ACTION_BUDDY::BuddyImportContactsResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyModifyNotify), ACTION_BUDDY::BuddyModifyNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyModifyReq), ACTION_BUDDY::BuddyModifyReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyModifyRes), ACTION_BUDDY::BuddyModifyResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyModifyUpdatePairNotify), ACTION_BUDDY::BuddyModifyUpdatePairNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyQueryListReq), ACTION_BUDDY::BuddyQueryListReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyQueryListRes), ACTION_BUDDY::BuddyQueryListResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyStatNotify), ACTION_BUDDY::BuddyStatNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgDownloadReq), ACTION_FILE::FileImgDownloadReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgDownloadRes), ACTION_FILE::FileImgDownloadResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgUploadReq), ACTION_FILE::FileImgUploadReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgUploadRes), ACTION_FILE::FileImgUploadResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupChangeMemberNotify), ACTION_GROUP::GroupChangeMemberNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupChangeMemberReq), ACTION_GROUP::GroupChangeMemberReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupChangeMemberRes), ACTION_GROUP::GroupChangeMemberResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupCreateReq), ACTION_GROUP::GroupCreateReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupCreateRes), ACTION_GROUP::GroupCreateResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetListReq), ACTION_GROUP::GroupGetListReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetListRes), ACTION_GROUP::GroupGetListResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetMembersListReq), ACTION_GROUP::GroupGetMembersListReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetMembersListRes), ACTION_GROUP::GroupGetMembersListResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupModifyNotify), ACTION_GROUP::GroupModifyNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupModifyReq), ACTION_GROUP::GroupModifyReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupModifyRes), ACTION_GROUP::GroupModifyResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupPreCreateReq), ACTION_GROUP::GroupPreCreateReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupPreCreateRes), ACTION_GROUP::GroupPreCreateResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionNotify), ACTION_GROUP::GroupRemoveSessionNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionReq), ACTION_GROUP::GroupRemoveSessionReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionRes), ACTION_GROUP::GroupRemoveSessionResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupUnreadMsgReq), ACTION_GROUP::GroupUnreadMsgReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupUnreadMsgRes), ACTION_GROUP::GroupUnreadMsgResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetByIdsReq), ACTION_MSG::MsgGetByIdsReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetByIdsRes), ACTION_MSG::MsgGetByIdsResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetMaxIdReq), ACTION_MSG::MsgGetMaxIdReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetMaxIdRes), ACTION_MSG::MsgGetMaxIdResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgNotify), ACTION_MSG::MsgNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgReadAckReq), ACTION_MSG::MsgReadAckReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgReadNotify), ACTION_MSG::MsgReadNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgReq), ACTION_MSG::MsgReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgRes), ACTION_MSG::MsgResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgUnNotify), ACTION_MSG::MsgUnNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_CaptchaReq), ACTION_OTHER::CaptchaReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_HeartBeatNotify), ACTION_OTHER::HeartBeatNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_ServerLoginReq), ACTION_SERVER::ServerLoginReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_ServerLoginRes), ACTION_SERVER::ServerLoginResAction));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchDevicesNotify), ACTION_SWITCH::SwitchDevicesNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchDevicesReq), ACTION_SWITCH::SwitchDevicesReqAction));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchPtpNotify), ACTION_SWITCH::SwitchPtpNotifyAction));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchPtpReq), ACTION_SWITCH::SwitchPtpReqAction));
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

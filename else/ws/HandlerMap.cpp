#include "HandlerMap.h"

#include "PTP.Common.pb.h"
#include "commands/Auth/AuthCaptcha.h"
#include "commands/Auth/AuthLogin.h"
#include "commands/Auth/AuthLogout.h"
#include "commands/Buddy/BuddyGetALL.h"
#include "commands/Buddy/BuddyGetList.h"
#include "commands/Buddy/BuddyGetStat.h"
#include "commands/Buddy/BuddyImportContacts.h"
#include "commands/Buddy/BuddyModifyNotify.h"
#include "commands/Buddy/BuddyModify.h"
#include "commands/Buddy/BuddyQueryList.h"
#include "commands/Buddy/BuddyStatNotify.h"
#include "commands/File/FileImgDownload.h"
#include "commands/File/FileImgUpload.h"
#include "commands/Group/GroupChangeMemberNotify.h"
#include "commands/Group/GroupChangeMember.h"
#include "commands/Group/GroupCreate.h"
#include "commands/Group/GroupGetList.h"
#include "commands/Group/GroupGetMembersList.h"
#include "commands/Group/GroupModifyNotify.h"
#include "commands/Group/GroupModify.h"
#include "commands/Group/GroupPreCreate.h"
#include "commands/Group/GroupRemoveSessionNotify.h"
#include "commands/Group/GroupRemoveSession.h"
#include "commands/Group/GroupUnreadMsg.h"
#include "commands/Msg/MsgGetByIds.h"
#include "commands/Msg/MsgGetMaxId.h"
#include "commands/Msg/MsgNotify.h"
#include "commands/Msg/MsgReadAck.h"
#include "commands/Msg/MsgReadNotify.h"
#include "commands/Msg/Msg.h"
#include "commands/Msg/MsgUnNotify.h"
#include "commands/Other/Captcha.h"
#include "commands/Other/HeartBeatNotify.h"
#include "commands/Server/ServerLogin.h"
#include "commands/Switch/SwitchDevicesNotify.h"
#include "commands/Switch/SwitchDevices.h"
#include "commands/Switch/SwitchPtpNotify.h"
#include "commands/Switch/SwitchPtp.h"

using namespace PTP::Common;

CHandlerMap* CHandlerMap::s_handler_instance = NULL;

/**
 *  构造函数
 */
CHandlerMap::CHandlerMap()
{

}

/**
 *  析构函数
 */
CHandlerMap::~CHandlerMap()
{

}

/**
 *  单例
 *
 *  @return 返回指向CHandlerMap的单例指针
 */
CHandlerMap* CHandlerMap::getInstance()
{
    if (!s_handler_instance) {
        s_handler_instance = new CHandlerMap();
        s_handler_instance->Init();
    }

    return s_handler_instance;
}

/**
 *  初始化函数,加载了各种commandId 对应的处理函数
 */
void CHandlerMap::Init()
{
    m_handler_map.insert(make_pair(uint32_t(CID_AuthCaptchaReq), COMMAND::AuthCaptchaReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_AuthLoginReq), COMMAND::AuthLoginReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_AuthLogoutReq), COMMAND::AuthLogoutReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetALLReq), COMMAND::BuddyGetALLReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetListReq), COMMAND::BuddyGetListReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyGetStatReq), COMMAND::BuddyGetStatReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyImportContactsReq), COMMAND::BuddyImportContactsReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyModifyReq), COMMAND::BuddyModifyReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_BuddyQueryListReq), COMMAND::BuddyQueryListReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgDownloadReq), COMMAND::FileImgDownloadReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_FileImgUploadReq), COMMAND::FileImgUploadReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupChangeMemberReq), COMMAND::GroupChangeMemberReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupCreateReq), COMMAND::GroupCreateReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetListReq), COMMAND::GroupGetListReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupGetMembersListReq), COMMAND::GroupGetMembersListReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupModifyReq), COMMAND::GroupModifyReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupPreCreateReq), COMMAND::GroupPreCreateReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionReq), COMMAND::GroupRemoveSessionReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_GroupUnreadMsgReq), COMMAND::GroupUnreadMsgReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetByIdsReq), COMMAND::MsgGetByIdsReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgGetMaxIdReq), COMMAND::MsgGetMaxIdReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgReadAckReq), COMMAND::MsgReadAckReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_MsgReq), COMMAND::MsgReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_CaptchaReq), COMMAND::CaptchaReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_ServerLoginReq), COMMAND::ServerLoginReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchDevicesReq), COMMAND::SwitchDevicesReqCmd));
    m_handler_map.insert(make_pair(uint32_t(CID_SwitchPtpReq), COMMAND::SwitchPtpReqCmd));
    
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyGetALLRes), COMMAND::BuddyGetALLResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyGetListRes), COMMAND::BuddyGetListResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyGetStatRes), COMMAND::BuddyGetStatResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyImportContactsRes), COMMAND::BuddyImportContactsResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyModifyNotify), COMMAND::BuddyModifyNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyModifyRes), COMMAND::BuddyModifyResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyQueryListRes), COMMAND::BuddyQueryListResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_BuddyStatNotify), COMMAND::BuddyStatNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_FileImgDownloadRes), COMMAND::FileImgDownloadResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_FileImgUploadRes), COMMAND::FileImgUploadResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupChangeMemberNotify), COMMAND::GroupChangeMemberNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupChangeMemberRes), COMMAND::GroupChangeMemberResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupCreateRes), COMMAND::GroupCreateResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupGetListRes), COMMAND::GroupGetListResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupGetMembersListRes), COMMAND::GroupGetMembersListResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupModifyNotify), COMMAND::GroupModifyNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupModifyRes), COMMAND::GroupModifyResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupPreCreateRes), COMMAND::GroupPreCreateResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionNotify), COMMAND::GroupRemoveSessionNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupRemoveSessionRes), COMMAND::GroupRemoveSessionResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_GroupUnreadMsgRes), COMMAND::GroupUnreadMsgResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgGetByIdsRes), COMMAND::MsgGetByIdsResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgGetMaxIdRes), COMMAND::MsgGetMaxIdResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgNotify), COMMAND::MsgNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgReadNotify), COMMAND::MsgReadNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgRes), COMMAND::MsgResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_MsgUnNotify), COMMAND::MsgUnNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_HeartBeatNotify), COMMAND::HeartBeatNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_ServerLoginRes), COMMAND::ServerLoginResCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_SwitchDevicesNotify), COMMAND::SwitchDevicesNotifyCmd));
    m_handler_callback_map.insert(make_pair(uint32_t(CID_SwitchPtpNotify), COMMAND::SwitchPtpNotifyCmd));
}

/**
 *  通过commandId获取处理函数
 *
 *  @param pdu_type commandId
 *
 *  @return 处理函数的函数指针
 */
pdu_handler_t CHandlerMap::GetHandler(uint32_t pdu_type)
{
    HandlerMap_t::iterator it = m_handler_map.find(pdu_type);
    if (it != m_handler_map.end()) {
        return it->second;
    } else {
        return NULL;
    }
}


/**
 *  通过commandId获取处理函数
 *
 *  @param pdu_type commandId
 *
 *  @return 处理函数的函数指针
 */
pdu_handler_callbakc_t CHandlerMap::GetHandlerCallback(uint32_t pdu_type)
{
    auto it = m_handler_callback_map.find(pdu_type);
    if (it != m_handler_callback_map.end()) {
        return it->second;
    } else {
        return NULL;
    }
}


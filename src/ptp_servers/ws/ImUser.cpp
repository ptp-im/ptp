#include "CachePool.h"
#include "ImUser.h"
#include "cache_common.h"
#include "IM.Server.pb.h"
#include "IM.Login.pb.h"


using namespace ::IM::BaseDefine;

CImUser::CImUser(string address_hex) {
    //DEBUG_I("ImUser, userId=%u\n", user_id);
    m_address_hex = address_hex;
    m_bValidate = false;
    m_user_id = 0;
    handle = 0;
    m_user_updated = false;
    m_pc_login_status = IM::BaseDefine::USER_STATUS_OFFLINE;
}


CImUser::~CImUser() {
    //DEBUG_I("~ImUser, userId=%u\n", m_user_id);
}

uint32_t CImUser::getMsgId(uint32_t msg_id) {
    for (auto it = m_msg_ids.begin(); it != m_msg_ids.end(); it++) {
        if (*it == msg_id) {
            return msg_id;
        }
    }

    return NULL;
}

CMsgConn *CImUser::GetUnValidateMsgConn(uint32_t handle) {
    for (set<CMsgConn *>::iterator it = m_unvalidate_conn_set.begin(); it != m_unvalidate_conn_set.end(); it++) {
        CMsgConn *pConn = *it;
        if (pConn->GetConnHandle() == handle) {
            return pConn;
        }
    }
    return NULL;
}

CMsgConn *CImUser::GetMsgConn(uint32_t handle) {
    CMsgConn *pMsgConn = NULL;
    map<uint32_t, CMsgConn *>::iterator it = m_conn_map.find(handle);
    if (it != m_conn_map.end()) {
        pMsgConn = it->second;
    }
    return pMsgConn;
}

void CImUser::ValidateMsgConn(uint32_t handle, CMsgConn *pMsgConn) {
    AddMsgConn(handle, pMsgConn);
    DelUnValidateMsgConn(pMsgConn);
}

user_conn_t CImUser::GetUserConn() {
    uint32_t conn_cnt = 0;
//    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
//        CMsgConn *pConn = it->second;
        // if (pConn->IsOpen()) {
        //     conn_cnt++;
        // }
//    }
    //  return NULL;
    user_conn_t user_cnt = {m_user_id, conn_cnt};
    return user_cnt;
}

void CImUser::BroadcastPdu(CImPdu *pPdu, CMsgConn *pFromConn) {
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pConn = it->second;
        if (pConn != pFromConn) {
            pConn->SendPdu(pPdu);
        }
    }
}

void CImUser::BroadcastPduWithOutMobile(CImPdu *pPdu, CMsgConn *pFromConn) {
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pConn = it->second;
        if (pConn != pFromConn && CHECK_CLIENT_TYPE_PC(pConn->GetClientType())) {
            pConn->SendPdu(pPdu);
        }
    }
}

void CImUser::BroadcastPduToMobile(CImPdu *pPdu, CMsgConn *pFromConn) {
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pConn = it->second;
        if (pConn != pFromConn && CHECK_CLIENT_TYPE_MOBILE(pConn->GetClientType())) {
            pConn->SendPdu(pPdu);
        }
    }
}


void CImUser::BroadcastClientMsgData(CImPdu *pPdu, uint32_t msg_id, CMsgConn *pFromConn, uint32_t from_id) {
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pConn = it->second;
        if (pConn != pFromConn) {
            pConn->SendPdu(pPdu);
            pConn->AddToSendList(msg_id, from_id);
        }
    }
}

void CImUser::BroadcastData(void *buff, uint32_t len, CMsgConn *pFromConn) {
    if (!buff)
        return;
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pConn = it->second;

        if (pConn == NULL)
            continue;

        if (pConn != pFromConn) {
              pConn->Send(buff, len);
        }
    }
}


void CImUser::GetDevicesPdu(PTP::Switch::SwitchDevicesNotify *msg_notify,CMsgConn *pIgnoreMsgConn) {
    map<uint32_t, CMsgConn *> &ConnMap = this->GetMsgConnMap();
    for (auto it1 = ConnMap.begin(); it1 != ConnMap.end(); it1++) {
        CMsgConn *pMsgConn1 = it1->second;
        if (pMsgConn1->GetHandle()) {
            if(nullptr != pIgnoreMsgConn && pIgnoreMsgConn==pMsgConn1){
                continue;
            }
            if(pMsgConn1->GetClientId().empty()){
                continue;
            }
            PTP::Common::DevicesInfo *device = msg_notify->add_devices();
            device->set_uid(pMsgConn1->GetUserId());
            device->set_client_id(pMsgConn1->GetClientId());
            device->set_os_name(pMsgConn1->GetOsName());
            device->set_os_version(pMsgConn1->GetOsVersion());
            device->set_browser_name(pMsgConn1->GetBrowserName());
            device->set_browser_version(pMsgConn1->GetBrowserVersion());
            device->set_is_intel(pMsgConn1->GetIsIntel());
            device->set_client_version(pMsgConn1->GetClientVersion());
            device->set_client_type((PTP::Common::ClientType)pMsgConn1->GetClientType());
            device->set_login_time(pMsgConn1->GetLoginTime());
        }
    }
}

void CImUser::GetDevicesPdu(PTP::Switch::SwitchDevicesNotify *msg_notify) {
    GetDevicesPdu(msg_notify, nullptr);
}

void CImUser::BroadcastDevicesPdu() {
    PTP::Switch::SwitchDevicesNotify msg_notify;
    this->GetDevicesPdu(&msg_notify);
    map<uint32_t, CMsgConn *> &ConnMap = this->GetMsgConnMap();
    for (auto it1 = ConnMap.begin(); it1 != ConnMap.end(); it1++) {
        CMsgConn *pMsgConn1 = it1->second;
        if (pMsgConn1->GetHandle()) {
            CImPdu pdu;
            pdu.SetPBMsg(&msg_notify);
            pdu.SetServiceId(S_SWITCH);
            pdu.SetCommandId(CID_SwitchDevicesNotify);
            pdu.SetSeqNum(0);
            pMsgConn1->SendPdu(&pdu);
        }
    }
}

void CImUser::BroadcastDevicesPduOnClose(CMsgConn *pFromConn) {
    PTP::Switch::SwitchDevicesNotify msg_notify;
    this->GetDevicesPdu(&msg_notify,pFromConn);
    map<uint32_t, CMsgConn *> &ConnMap = this->GetMsgConnMap();
    for (auto it1 = ConnMap.begin(); it1 != ConnMap.end(); it1++) {
        CMsgConn *pMsgConn1 = it1->second;
        if (pMsgConn1->GetHandle() && pFromConn != pMsgConn1) {
            CImPdu pdu;
            pdu.SetPBMsg(&msg_notify);
            pdu.SetServiceId(S_SWITCH);
            pdu.SetCommandId(CID_SwitchDevicesNotify);
            pdu.SetSeqNum(0);
            pMsgConn1->SendPdu(&pdu);
        }
    }
}
void CImUser::HandleKickUser(CMsgConn* pConn, uint32_t reason)
{
    map<uint32_t, CMsgConn*>::iterator it = m_conn_map.find(pConn->GetHandle());
    if (it != m_conn_map.end()) {
        CMsgConn* pConn = it->second;
        if(pConn) {
            DEBUG_I("kick service user, user_id=%u.handle: %d,uid: %d", m_user_id,pConn->GetHandle(),pConn->GetUserId());
            IM::Login::IMKickUser msg;
            msg.set_user_id(m_user_id);
            msg.set_kick_reason((::IM::BaseDefine::KickReasonType)reason);
            CImPdu pdu;
            pdu.SetPBMsg(&msg);
            pdu.SetServiceId(SID_LOGIN);
            pdu.SetCommandId(CID_LOGIN_KICK_USER);
            pConn->SendPdu(&pdu);
            pConn->SetKickOff();
            //pConn->Close();
        }
    }
}

// 只支持一个WINDOWS/MAC客户端登陆,或者一个ios/android登录
bool CImUser::KickOutSameClientType(uint32_t client_type, uint32_t reason, CMsgConn *pFromConn) {
    for (map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin(); it != m_conn_map.end(); it++) {
        CMsgConn *pMsgConn = it->second;
        //16进制位移计算
        if ((((pMsgConn->GetClientType() ^ client_type) >> 4) == 0) && (pMsgConn != pFromConn)) {
            HandleKickUser(pMsgConn, reason);
            break;
        }
    }
    return true;
}

uint32_t CImUser::GetClientTypeFlag() {
    uint32_t client_type_flag = 0x00;
    map<uint32_t, CMsgConn *>::iterator it = m_conn_map.begin();
    for (; it != m_conn_map.end(); it++) {
        CMsgConn* pConn = it->second;
        uint32_t client_type = pConn->GetClientType();
        if (CHECK_CLIENT_TYPE_PC(client_type))
        {
            client_type_flag |= CLIENT_TYPE_FLAG_PC;
        }
        else if (CHECK_CLIENT_TYPE_MOBILE(client_type))
        {
            client_type_flag |= CLIENT_TYPE_FLAG_MOBILE;
        }
    }
    return client_type_flag;
}


CImUserManager::~CImUserManager() {
    RemoveAll();
}

CImUserManager *CImUserManager::GetInstance() {
    static CImUserManager s_manager;
    return &s_manager;
}

CImUser *CImUserManager::GetImUserById(uint32_t user_id) {
    CImUser *pUser = NULL;
    ImUserMap_t::iterator it = m_im_user_map.find(user_id);
    if (it != m_im_user_map.end()) {
        pUser = it->second;
    }
    return pUser;
}

CImUser *CImUserManager::GetImUserByAddressHex(string address_hex) {

    CImUser *pUser = NULL;
    ImUserMapByAddressHex_t::iterator it = m_im_user_address_hex_map.find(address_hex);
    if (it != m_im_user_address_hex_map.end()) {
        pUser = it->second;
    }
    return pUser;

}

CMsgConn *CImUserManager::GetMsgConnByHandle(uint32_t user_id, uint32_t handle) {
    CMsgConn *pMsgConn = NULL;
    CImUser *pImUser = GetImUserById(user_id);
    if (pImUser) {
        pMsgConn = pImUser->GetMsgConn(handle);
    }
    return pMsgConn;
}

void CImUserManager::RemoveImUserByAddressHex(std::string address_hex) {
    m_im_user_address_hex_map.erase(address_hex);
}

bool CImUserManager::AddImUserById(uint32_t user_id, CImUser *pUser) {
    bool bRet = false;
    if (GetImUserById(user_id) == NULL) {
        m_im_user_map[user_id] = pUser;
        bRet = true;
    }
    return bRet;
}

bool CImUserManager::AddImUserByAddressHex(string address_hex, CImUser *pUser) {
    bool bRet = false;
    if (GetImUserByAddressHex(address_hex) == NULL) {
        m_im_user_address_hex_map[address_hex] = pUser;
        bRet = true;
    }
    return bRet;
}

void CImUserManager::RemoveImUserById(uint32_t user_id) {
    m_im_user_map.erase(user_id);
}

void CImUserManager::RemoveImUser(CImUser *pUser) {
    if (pUser != NULL) {
        RemoveImUserById(pUser->GetUserId());
        RemoveImUserByAddressHex(pUser->GetAddressHex());
        if(pUser != NULL){
            delete pUser;
            pUser = NULL;
        }
    }
}

void CImUserManager::RemoveAll() {
    for (ImUserMapByAddressHex_t ::iterator it = m_im_user_address_hex_map.begin(); it != m_im_user_address_hex_map.end();
         it++) {
        CImUser *pUser = it->second;
        if (pUser != NULL) {
            delete pUser;
            pUser = NULL;
        }
    }
    m_im_user_address_hex_map.clear();
    m_im_user_map.clear();
}

void CImUserManager::GetOnlineUserInfo(list<user_stat_t> *online_user_info) {
    user_stat_t status;
    CImUser *pImUser = NULL;
    for (ImUserMap_t::iterator it = m_im_user_map.begin(); it != m_im_user_map.end(); it++) {
        pImUser = (CImUser *) it->second;
        if (pImUser->IsValidate()) {
            map<uint32_t, CMsgConn *> &ConnMap = pImUser->GetMsgConnMap();
            for (map<uint32_t, CMsgConn *>::iterator it = ConnMap.begin(); it != ConnMap.end(); it++) {
                CMsgConn *pConn = it->second;
                if (pConn->IsOpen())
                {
                    status.user_id = pImUser->GetUserId();
                    status.client_type = pConn->GetClientType();
                    status.status = pConn->GetOnlineStatus();
                    online_user_info->push_back(status);
                }
            }
        }
    }
}

void CImUserManager::GetUserConnCnt(list<user_conn_t> *user_conn_list, uint32_t &total_conn_cnt) {
    total_conn_cnt = 0;
    CImUser *pImUser = NULL;
    for (ImUserMap_t::iterator it = m_im_user_map.begin(); it != m_im_user_map.end(); it++) {
        pImUser = (CImUser *) it->second;
        if (pImUser->IsValidate()) {
            user_conn_t user_conn_cnt = pImUser->GetUserConn();
            user_conn_list->push_back(user_conn_cnt);
            total_conn_cnt += user_conn_cnt.conn_cnt;
        }
    }
}

void CImUserManager::BroadcastPdu(CImPdu *pdu, uint32_t client_type_flag) {
    CImUser *pImUser = NULL;
    for (ImUserMap_t::iterator it = m_im_user_map.begin(); it != m_im_user_map.end(); it++) {
        pImUser = (CImUser *) it->second;
        if (pImUser->IsValidate()) {
            switch (client_type_flag) {
                case CLIENT_TYPE_FLAG_PC:
                    pImUser->BroadcastPduWithOutMobile(pdu);
                    break;
                case CLIENT_TYPE_FLAG_MOBILE:
                    pImUser->BroadcastPduToMobile(pdu);
                    break;
                case CLIENT_TYPE_FLAG_BOTH:
                    pImUser->BroadcastPdu(pdu);
                    break;
                default:
                    break;
            }
        }
    }
}

uint32_t CImUser::InitCache(){
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("auth");
    if (pCacheConn) {
        uint32_t user_id;
        string address_hex = m_address_hex;
        auto user_id_str= pCacheConn->get(CACHE_USER_ADR_ID_PREFIX + address_hex);
        if(user_id_str.empty()){
            auto user_id_long = pCacheConn->incr(CACHE_INCR_USER_ID);
            if(user_id_long == 1){
                user_id_long = pCacheConn->incrBy(CACHE_INCR_USER_ID,99999);
            }
            list<string> argv;
            string user_id_str1 = int2string(user_id_long);
            argv.emplace_back("MSET");
            argv.push_back(CACHE_USER_ADR_ID_PREFIX + address_hex);
            argv.push_back(user_id_str1);
            argv.push_back(CACHE_USER_ID_PUB_KEY_PREFIX + user_id_str1);
            argv.push_back(m_pub_key_64);
            argv.push_back(CACHE_USER_ID_ADR_PREFIX + user_id_str1);
            argv.push_back(address_hex);
            pCacheConn->exec(&argv, nullptr);
            user_id = (uint32_t)(atoi(user_id_str1.c_str()));
        }else{
            user_id = (uint32_t)(atoi(user_id_str.c_str()));
        }
        pCacheManager->RelCacheConn(pCacheConn);
        DEBUG_I("user_id: %d -> %s", user_id,address_hex.c_str());
        return user_id;
    }else{
        DEBUG_E("error pCacheConn");
        return 0;
    }
}

uint32_t GetUserIdFromCache(string address,bool gen_user_id)
{
    CacheManager* pCacheManager = CacheManager::getInstance();
    CacheConn* pCacheConn = pCacheManager->GetCacheConn("auth");
    if (pCacheConn) {
        uint32_t user_id;
        auto user_id_str= pCacheConn->get(CACHE_USER_ADR_ID_PREFIX+address);
        if(user_id_str.empty()){
            if(!gen_user_id){
                return 0;
            }
            auto user_id_long = pCacheConn->incr(CACHE_INCR_USER_ID);
            if(user_id_long == 1){
                user_id_long = pCacheConn->incrBy(CACHE_INCR_USER_ID,99999);
            }
            auto user_id_str1 = to_string(user_id_long);
            pCacheConn->set(CACHE_USER_ADR_ID_PREFIX+address, user_id_str1);
            pCacheConn->set(CACHE_USER_ID_ADR_PREFIX+user_id_str1, address);
            user_id = (uint32_t)(atoi(user_id_str1.c_str()));
        }else{
            user_id = (uint32_t)(atoi(user_id_str.c_str()));
        }
        pCacheManager->RelCacheConn(pCacheConn);
        DEBUG_I("%s -> %d", address.c_str(),user_id);
        return user_id;
    }else{
        DEBUG_E("error pCacheConn");
        return 0;
    }
}


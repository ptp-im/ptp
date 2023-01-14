#include "ImUser.h"
//#include "IM.Login.pb.h"
#include "CachePool.h"

//using namespace ::IM::BaseDefine;

CImUser::CImUser(const string &address_hex) {
    m_address_hex = address_hex;
    m_user_id = 0;
    m_handle = 0;
}

CImUser::~CImUser() = default;

CMsgSrvConn *CImUser::GetMsgConn(uint32_t handle) {
    CMsgSrvConn *pMsgConn = nullptr;
    auto it = m_conn_map.find(handle);
    if (it != m_conn_map.end()) {
        pMsgConn = it->second;
    }
    return pMsgConn;
}

user_conn_t CImUser::GetUserConn() {
    uint32_t conn_cnt = 0;
    for (auto & it : m_conn_map) {
        CMsgSrvConn *pConn = it.second;
         if (pConn->IsOpen()) {
             conn_cnt++;
         }
    }
    user_conn_t user_cnt = {m_user_id, conn_cnt};
    return user_cnt;
}

void CImUser::BroadcastPdu(ImPdu *pPdu, CMsgSrvConn *pFromConn) {
    for (auto & it : m_conn_map) {
        CMsgSrvConn *pConn = it.second;
        if (pConn != pFromConn && pConn->IsOpen()) {
            pConn->SendPdu(pPdu);
        }
    }
}

void CImUser::BroadcastData(void *buff, uint32_t len, CMsgSrvConn *pFromConn) {
    if (!buff)
        return;
    for (auto & it : m_conn_map) {
        CMsgSrvConn *pConn = it.second;

        if (pConn == nullptr)
            continue;

        if (pConn != pFromConn && pConn->IsOpen()) {
              pConn->Send(buff, (int)len);
        }
    }
}

CImUserManager::~CImUserManager() {
    RemoveAll();
}

CImUserManager *CImUserManager::GetInstance() {
    static CImUserManager s_manager;
    return &s_manager;
}

CImUser *CImUserManager::GetImUserById(uint32_t user_id) {
    CImUser *pUser = nullptr;
    auto it = m_im_user_id_map.find(user_id);
    if (it != m_im_user_id_map.end()) {
        pUser = it->second;
    }
    return pUser;
}

CImUser *CImUserManager::GetImUserByAddress(string address) {

    CImUser *pUser = nullptr;
    auto it = m_im_user_address_map.find(address);
    if (it != m_im_user_address_map.end()) {
        pUser = it->second;
    }
    return pUser;

}

CMsgSrvConn *CImUserManager::GetMsgConnByHandle(uint32_t user_id, uint32_t handle) {
    CMsgSrvConn *pMsgConn = nullptr;
    CImUser *pImUser = GetImUserById(user_id);
    if (pImUser) {
        pMsgConn = pImUser->GetMsgConn(handle);
    }
    return pMsgConn;
}

void CImUserManager::RemoveImUserByAddress(std::string address) {
    m_im_user_address_map.erase(address);
}

bool CImUserManager::AddImUserById(uint32_t user_id, CImUser *pUser) {
    bool bRet = false;
    if (GetImUserById(user_id) == nullptr) {
        m_im_user_id_map[user_id] = pUser;
        bRet = true;
    }
    return bRet;
}

bool CImUserManager::AddImUserByAddress(string address, CImUser *pUser) {
    bool bRet = false;
    if (GetImUserByAddress(address) == nullptr) {
        m_im_user_address_map[address] = pUser;
        bRet = true;
    }
    return bRet;
}

void CImUserManager::RemoveImUserById(uint32_t user_id) {
    m_im_user_id_map.erase(user_id);
}

void CImUserManager::RemoveImUser(CImUser *pUser) {
    if (pUser != nullptr) {
        RemoveImUserById(pUser->GetUserId());
        RemoveImUserByAddress(pUser->GetAddress());
        if(pUser != nullptr){
            delete pUser;
            pUser = nullptr;
        }
    }
}

void CImUserManager::RemoveAll() {
    for (auto & it : m_im_user_id_map) {
        CImUser *pUser = it.second;
        if (pUser != nullptr) {
            delete pUser;
            pUser = nullptr;
        }
    }
    m_im_user_address_map.clear();
    m_im_user_id_map.clear();
}

void CImUserManager::GetOnlineUserInfo(list<user_stat_t> *online_user_info) {
    user_stat_t status;
    CImUser *pImUser = nullptr;
    for (auto & it : m_im_user_id_map) {
        pImUser = (CImUser *) it.second;
        map<uint32_t, CMsgSrvConn *> &ConnMap = pImUser->GetMsgConnMap();
        for (auto & it1 : ConnMap) {
            CMsgSrvConn *pConn = it1.second;
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

void CImUserManager::GetUserConnCnt(list<user_conn_t> *user_conn_list, uint32_t &total_conn_cnt) {
    total_conn_cnt = 0;
    CImUser *pImUser = nullptr;
    for (auto & it : m_im_user_id_map) {
        pImUser = (CImUser *) it.second;
        user_conn_t user_conn_cnt = pImUser->GetUserConn();
        user_conn_list->push_back(user_conn_cnt);
        total_conn_cnt += user_conn_cnt.conn_cnt;
    }
}

void CImUserManager::BroadcastPdu(ImPdu *pdu, uint32_t client_type_flag) {
    CImUser *pImUser = nullptr;
    for (auto & it : m_im_user_id_map) {
        pImUser = (CImUser *) it.second;
        pImUser->BroadcastPdu(pdu);
    }
}

#ifndef __MSG_IMUSER_H__
#define __MSG_IMUSER_H__

#include <utility>

#include "ptp_global/ImConn.h"
#include "ptp_global/global_define.h"
#include "ptp_protobuf/ImPdu.h"
#import "MsgSrvConn.h"
#include "PTP.Switch.pb.h"

class CMsgSrvConn;

class CImUser {
public:
    explicit CImUser(const string & address_hex);

    ~CImUser();
    user_conn_t GetUserConn();
    void SetHandle(uint32_t handle) { m_handle = handle; }
    void SetUserId(uint32_t user_id) { m_user_id = user_id; }
    void SetPubKey(string pub_key) { m_pub_key_64 = std::move(pub_key); }
    void SetAddress(string address) { m_address_hex = std::move(address); }

    string GetAddress() { return m_address_hex; }
    uint32_t GetUserId() const { return m_user_id; }
    string GetPubKey() const { return m_pub_key_64; }
    uint32_t GetHandle() const { return m_handle; }

    bool IsMsgConnEmpty() { return m_conn_map.empty(); }
    void AddMsgConn(uint32_t handle, CMsgSrvConn *pMsgConn) { m_conn_map[handle] = pMsgConn; }
    void DelMsgConn(uint32_t handle) { m_conn_map.erase(handle); }
    CMsgSrvConn *GetMsgConn(uint32_t handle);
    map<uint32_t, CMsgSrvConn *> &GetMsgConnMap() { return m_conn_map; }
    void BroadcastPdu(ImPdu *pPdu, CMsgSrvConn *pFromConn = nullptr);
    void BroadcastData(void *buff, uint32_t len, CMsgSrvConn *pFromConn = nullptr);

private:
    uint32_t m_user_id;
    string m_address_hex;
    string m_pub_key_64;
    uint32_t m_handle;
    map<uint32_t /* handle */, CMsgSrvConn *> m_conn_map;
};

typedef map<uint32_t /* user_id */, CImUser *> ImUserIdMap_t;
typedef map<string /* address */, CImUser *> ImUserAddressMap_t;

class CImUserManager {
public:
    CImUserManager() = default;
    ~CImUserManager();
    static CImUserManager *GetInstance();

    ImUserIdMap_t GetUserMap(){ return m_im_user_id_map; }
    ImUserAddressMap_t GetUserAddressMap(){ return m_im_user_address_map; }
    CImUser *GetImUserById(uint32_t user_id);
    CImUser *GetImUserByAddress(string address_hex);
    CMsgSrvConn *GetMsgConnByHandle(uint32_t user_id, uint32_t handle);
    bool AddImUserByAddress(string address, CImUser *pUser);
    void RemoveImUserByAddress(string address);
    bool AddImUserById(uint32_t user_id, CImUser *pUser);
    void RemoveImUserById(uint32_t user_id);
    void RemoveImUser(CImUser *pUser);
    void RemoveAll();
    void GetOnlineUserInfo(list<user_stat_t> *online_user_info);
    void GetUserConnCnt(list<user_conn_t> *user_conn_list, uint32_t &total_conn_cnt);
    void BroadcastPdu(ImPdu *pdu, uint32_t client_type_flag);

private:
    ImUserIdMap_t m_im_user_id_map;
    ImUserAddressMap_t m_im_user_address_map;
};

#endif /* __MSG_IMUSER_H__ */

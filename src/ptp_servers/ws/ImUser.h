/*
 * ImUser.h
 *
 *  Created on: 2014年4月16日
 *      Author: ziteng
 */

#ifndef IMUSER_H_
#define IMUSER_H_

#include "ptp_global/ImConn.h"
#include "public_define.h"
#import "MsgConn.h"
#include "PTP.Switch.pb.h"

#define MAX_ONLINE_FRIEND_CNT        100    //通知好友状态通知的最多个数

class CMsgConn;

class CImUser {
public:
    CImUser(string address_hex);

    ~CImUser();

    void SetHandle(uint32_t m_handle) { handle = m_handle; }
    void SetUserId(uint32_t user_id) { m_user_id = user_id; }
    void SetPubKey(string pub_key) { m_pub_key_64 = pub_key; }
    void SetAddressHex(string address_hex) { m_address_hex = address_hex; }
    void SetNickName(string nick_name) { m_nick_name = nick_name; }
    void SetValidated() { m_bValidate = true; }
    void SetPCLoginStatus(uint32_t pc_login_status) { m_pc_login_status = pc_login_status; }

    string GetAddressHex() { return m_address_hex; }
    string GetNickName() { return m_nick_name; }
    uint32_t GetUserId() const { return m_user_id; }
    string GetPubKey() const { return m_pub_key_64; }
    uint32_t GetHandle() const { return handle; }
    uint32_t GetPCLoginStatus() const { return m_pc_login_status; }
    set<CMsgConn *> GetUnValidConn() const { return m_unvalidate_conn_set; }
    bool IsValidate() const { return m_bValidate; }
    bool IsMsgConnEmpty() { return m_conn_map.empty(); }

    user_conn_t GetUserConn();

    uint32_t InitCache();
    void AddMsgConn(uint32_t handle, CMsgConn *pMsgConn) { m_conn_map[handle] = pMsgConn; }
    void DelMsgConn(uint32_t handle) { m_conn_map.erase(handle); }
    void AddUnValidateMsgConn(CMsgConn *pMsgConn) { m_unvalidate_conn_set.insert(pMsgConn); }
    void ValidateMsgConn(uint32_t handle, CMsgConn *pMsgConn);
    void DelUnValidateMsgConn(CMsgConn *pMsgConn) { m_unvalidate_conn_set.erase(pMsgConn); }

    CMsgConn *GetUnValidateMsgConn(uint32_t handle);
    CMsgConn *GetMsgConn(uint32_t handle);

    void AddMsgId(uint32_t msg_id) { m_msg_ids.insert(msg_id); }
    uint32_t getMsgId(uint32_t msg_id);

    map<uint32_t, CMsgConn *> &GetMsgConnMap() { return m_conn_map; }

    void GetDevicesPdu(PTP::Switch::SwitchDevicesNotify *msg_rsp);
    void GetDevicesPdu(PTP::Switch::SwitchDevicesNotify *msg_rsp, CMsgConn *pIgnoreMsgConn);

    void BroadcastPdu(CImPdu *pPdu, CMsgConn *pFromConn = NULL);
    void BroadcastDevicesPdu();
    void BroadcastDevicesPduOnClose(CMsgConn *pFromConn);
    void BroadcastPduWithOutMobile(CImPdu *pPdu, CMsgConn *pFromConn = NULL);

    void BroadcastPduToMobile(CImPdu *pPdu, CMsgConn *pFromConn = NULL);

    void BroadcastClientMsgData(CImPdu *pPdu, uint32_t msg_id, CMsgConn *pFromConn = NULL, uint32_t from_id = 0);

    void BroadcastData(void *buff, uint32_t len, CMsgConn *pFromConn = NULL);

    void HandleKickUser(CMsgConn *pConn, uint32_t reason);

    bool KickOutSameClientType(uint32_t client_type, uint32_t reason, CMsgConn *pFromConn = NULL);

    uint32_t GetClientTypeFlag();

private:
    uint32_t m_user_id;
    string m_address_hex;
    string m_pub_key_64;
    uint32_t handle;
    string m_nick_name;            /* 花名 */
    bool m_user_updated;
    uint32_t m_pc_login_status;  // pc client login状态，1: on 0: off
    bool m_bValidate;

    map<uint32_t /* handle */, CMsgConn *> m_conn_map;
    set<CMsgConn *> m_unvalidate_conn_set;
    set<uint32_t> m_msg_ids;
};

typedef map<uint32_t /* user_id */, CImUser *> ImUserMap_t;
typedef map<string /* address */, CImUser *> ImUserMapByAddressHex_t;

class CImUserManager {
public:
    CImUserManager() {}

    ~CImUserManager();

    static CImUserManager *GetInstance();

    ImUserMap_t GetUserMap(){ return m_im_user_map; }
    ImUserMapByAddressHex_t GetUserAddressMap(){ return m_im_user_address_hex_map; }

    CImUser *GetImUserById(uint32_t user_id);

    CImUser *GetImUserByAddressHex(string address_hex);

    CMsgConn *GetMsgConnByHandle(uint32_t user_id, uint32_t handle);

    bool AddImUserByAddressHex(string address_hex, CImUser *pUser);

    void RemoveImUserByAddressHex(string address_hex);

    bool AddImUserById(uint32_t user_id, CImUser *pUser);

    void RemoveImUserById(uint32_t user_id);

    void RemoveImUser(CImUser *pUser);

    void RemoveAll();

    void GetOnlineUserInfo(list<user_stat_t> *online_user_info);

    void GetUserConnCnt(list<user_conn_t> *user_conn_list, uint32_t &total_conn_cnt);

    void BroadcastPdu(CImPdu *pdu, uint32_t client_type_flag);

private:
    ImUserMap_t m_im_user_map;
    ImUserMapByAddressHex_t m_im_user_address_hex_map;
};

void get_online_user_info(list<user_stat_t> *online_user_info);

uint32_t GetUserIdFromCache(string address,bool gen_user_id);

#endif /* IMUSER_H_ */

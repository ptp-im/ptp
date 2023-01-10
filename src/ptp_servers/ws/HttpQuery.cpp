#include <sstream>
#include "HttpQuery.h"
#include "RouteServConn.h"
#include "HttpPdu.h"
#include "ptp_global/global_define.h"
#include "ptp_global/AttachData.h"
#include "DBServConn.h"
#include "ImUser.h"
#include "IM.Message.pb.h"
#include "IM.Group.pb.h"
#include "IM.Other.pb.h"

static uint32_t g_total_query = 0;
static uint32_t g_last_year = 0;
static uint32_t g_last_month = 0;
static uint32_t g_last_mday = 0;
#include <unordered_map>

CHttpQuery *CHttpQuery::m_query_instance = NULL;

unordered_map<string, auth_struct *> g_hm_http_auth;

void http_query_timer_callback(void *callback_data, uint8_t msg, uint32_t handle, void *pParam) {
    struct tm *tm;
    time_t currTime;

    time(&currTime);
    tm = localtime(&currTime);

    uint32_t year = tm->tm_year + 1900;
    uint32_t mon = tm->tm_mon + 1;
    uint32_t mday = tm->tm_mday;
    if (year != g_last_year || mon != g_last_month || mday != g_last_mday) {
        // a new day begin, clear the count
        DEBUG_I("a new day begin, g_total_query=%u ", g_total_query);
        g_total_query = 0;
        g_last_year = year;
        g_last_month = mon;
        g_last_mday = mday;
    }
}

HTTP_ERROR_CODE CHttpQuery::_CheckAuth(const string &strAppKey, const uint32_t userId, const string &strInterface,
                                       const string &strIp) {
    return HTTP_ERROR_SUCCESS;
}

HTTP_ERROR_CODE
CHttpQuery::_CheckPermission(const string &strAppKey, uint8_t nType, const list<uint32_t> &lsToId, string strMsg) {
    strMsg.clear();
    return HTTP_ERROR_SUCCESS;
}

CHttpQuery *CHttpQuery::GetInstance() {
    if (!m_query_instance) {
        m_query_instance = new CHttpQuery();
        netlib_register_timer(http_query_timer_callback, NULL, 1000);
    }
    return m_query_instance;
}

void CHttpQuery::_ApiSendMsg(const string &strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn) {
    CDBServConn *pConn = get_db_serv_conn();
    if (!pConn) {
        DEBUG_I("no connection to dbServConn ");
        char *response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if (
            post_json_obj["fromUserId"].isNull() ||
            post_json_obj["toSessionId"].isNull() ||
            post_json_obj["sessionType"].isNull() ||
            post_json_obj["msgType"].isNull() ||
            post_json_obj["msgData"].isNull()
            ) {
        DEBUG_I("send msg 参数不完整.");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if (!IM::BaseDefine::SessionType_IsValid(post_json_obj["sessionType"].asInt())) {
        return;
    }

    if (!IM::BaseDefine::MsgType_IsValid(post_json_obj["msgType"].asInt())) {
        return;
    }

    IM::Message::IMMsgData msg;
    std::string msgData = post_json_obj["msgData"].asString();

    msg.set_msg_data(msgData);
    msg.set_msg_id(0);
    msg.set_msg_type((IM::BaseDefine::MsgType) post_json_obj["msgType"].asInt());
    msg.set_from_user_id(post_json_obj["fromUserId"].asInt());
//    msg.set_to_session_id(post_json_obj["toSessionId"].asInt());

//    msg.set_session_type((IM::BaseDefine::SessionType)post_json_obj["sessionType"].asInt());
    uint32_t cur_time = time(NULL);
    msg.set_create_time(cur_time);

    CImPdu pdu;
    CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
    msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(IM::BaseDefine::SID_MSG);
    pdu.SetCommandId(IM::BaseDefine::CID_MSG_DATA);
    // send to DB storage server
    if (pConn) {
        pConn->SendPdu(&pdu);
    }
    char *response_buf = NULL;
    response_buf = PackSendResult(HTTP_ERROR_SUCCESS, HTTP_ERROR_MSG[0].c_str());
    pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
    pHttpConn->Close();
}


void CHttpQuery::_PushNotice(const string &strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn) {
    CDBServConn *pConn = get_db_serv_conn();
    if (!pConn) {
        DEBUG_I("no connection to CDBServConn ");
        char *response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    if (post_json_obj["userId"].isNull()) {
        DEBUG_I("no user id ");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if (post_json_obj["notice_action"].isNull()) {
        DEBUG_I("no notice_action ");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if (post_json_obj["notice_data"].isNull()) {
        DEBUG_I("no notice_action ");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if (post_json_obj["members"].isNull()) {
        DEBUG_I("no user list ");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    try {
        uint32_t user_id = post_json_obj["userId"].asUInt();
        string notice_action = post_json_obj["notice_action"].asString();
        string notice_data = post_json_obj["notice_data"].asString();
        uint32_t user_cnt = post_json_obj["members"].size();
        DEBUG_I("_PushNotice, userId: %u, action: %s, data: %s", user_id, notice_action.c_str(), notice_data.c_str());

        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
//        IM::Other::IMPushNoticeReq msg;
//        msg.set_user_id(user_id);
//        msg.set_notice_data(notice_data);
//        msg.set_notice_action(notice_action);
//        msg.set_user_id(user_id);
//        for (uint32_t i = 0; i < user_cnt; i++) {
//            uint32_t member_id = post_json_obj["members"][i].asUInt();
//            msg.add_members(member_id);
//        }
//        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
//        CImPdu pdu;
//        pdu.SetPBMsg(&msg);
//        pdu.SetServiceId(IM::BaseDefine::SID_OTHER);
//        pdu.SetCommandId(IM::BaseDefine::CID_OTHER_PUSH_NOTICE_REQ);
//        pConn->SendPdu(&pdu);
    }
    catch (std::runtime_error msg) {
        DEBUG_I("parse json data failed.");
        char *response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t) strlen(response_buf));
        pHttpConn->Close();
    }
}

void CHttpQuery::DispatchQuery(std::string &url, std::string &post_data, CHttpConn *pHttpConn) {
    ++g_total_query;
    if (strcmp(url.c_str(), "/api/status") == 0) {
        _ApiStatus(pHttpConn);
        return;
    }

    Json::Value root;
    DEBUG_I("DispatchQuery, url=%s, content=%s ", url.c_str(), post_data.c_str());

    Json::Value value;
    Json::Reader *reader = new Json::Reader(Json::Features::strictMode());

    if (!reader->parse(post_data, value)) {
        DEBUG_I("json parse failed, post_data=%s ", post_data.c_str());
        pHttpConn->Close();
        return;
    }

    string strErrorMsg;
    string strAppKey;
    HTTP_ERROR_CODE nRet = HTTP_ERROR_SUCCESS;
    try {
        string strInterface(url.c_str() + strlen("/api/"));
        strAppKey = value["apiKey"].asString();
        string strIp = pHttpConn->GetPeerIP();
        uint32_t nUserId = value["userId"].asUInt();
        nRet = _CheckAuth(strAppKey, nUserId, strInterface, strIp);
    }
    catch (std::runtime_error msg) {
        nRet = HTTP_ERROR_INTERFACE;
    }

    if (HTTP_ERROR_SUCCESS != nRet) {
        if (nRet < HTTP_ERROR_MAX) {
            root["error_code"] = nRet;
            root["error_msg"] = HTTP_ERROR_MSG[nRet];
        }
        string strResponse = root.toStyledString();
        pHttpConn->Send((void *) strResponse.c_str(), strResponse.length());
        return;
    }

    if (strcmp(url.c_str(), "/api/ChangeMembers") == 0) {
    } else if (strcmp(url.c_str(), "/api/PushNotice") == 0) {
        _PushNotice(strAppKey, value, pHttpConn);
    } else if (strcmp(url.c_str(), "/api/sendImMsg") == 0) {
        _ApiSendMsg(strAppKey, value, pHttpConn);
    } else {
        DEBUG_I("url not support ");
        pHttpConn->Close();
        return;
    }
}

void CHttpQuery::_ApiStatus(CHttpConn *pHttpConn) {
    Json::Value root;
    list<user_stat_t> online_user_list;

    auto m_im_user_map = CImUserManager::GetInstance()->GetUserMap();
    Json::Value users;
    for (auto it = m_im_user_map.begin(); it != m_im_user_map.end(); it++) {
        auto pImUser = (CImUser *) it->second;
        if (pImUser) {
            Json::Value user;
            Json::Value msgConns;
            map<uint32_t, CMsgConn *> &ConnMap = pImUser->GetMsgConnMap();
            for (auto it1 = ConnMap.begin(); it1 != ConnMap.end(); it1++) {
                CMsgConn *pMsgConn = it1->second;
                if (pMsgConn->GetHandle()) {
                    Json::Value jsonMsgConn;
                    jsonMsgConn["user_id"] = pMsgConn->GetUserId();
                    jsonMsgConn["login_time"] = pMsgConn->GetLoginTime();
                    jsonMsgConn["client_type"] = pMsgConn->GetClientType();
                    jsonMsgConn["client_id"] = pMsgConn->GetClientId();
                    jsonMsgConn["os_name"] = pMsgConn->GetOsName();
                    jsonMsgConn["browser_name"] = pMsgConn->GetBrowserName();
                    jsonMsgConn["online_status"] = pMsgConn->GetOnlineStatus();
                    jsonMsgConn["client_version"] = pMsgConn->GetClientVersion().c_str();
                    jsonMsgConn["isOpen"] = pMsgConn->IsOpen();
                    jsonMsgConn["state"] = pMsgConn->GetState();
                    jsonMsgConn["isKickOff"] = pMsgConn->IsKickOff();
                    msgConns[to_string(pMsgConn->GetHandle())] = jsonMsgConn;
                }
            }
            user["addr"] = pImUser->GetAddressHex();
            user["clientType"] = pImUser->GetClientTypeFlag();
            user["pcLoginStatus"] = pImUser->GetPCLoginStatus();
            user["isValidate"] = pImUser->IsValidate();
            user["isMsgConnEmpty"] = pImUser->IsMsgConnEmpty();
            user["msgConns"] = msgConns;

            Json::Value UnValidConns;
            auto k = 0;
            auto UnValidConn = pImUser->GetUnValidConn();
            for (auto it2 = UnValidConn.begin(); it2 != UnValidConn.end(); it2++) {
                CMsgConn *pConn = *it2;
                UnValidConns[k] = pConn->GetConnHandle();
                k++;
            }
            user["UnValidConns"] = UnValidConns;
            users[to_string(pImUser->GetUserId())] = user;
        }

    }
    root["users"] = users;

    string strResponse = root.toStyledString();
    pHttpConn->Send((void *) strResponse.c_str(), strResponse.length());
    pHttpConn->Close();
}
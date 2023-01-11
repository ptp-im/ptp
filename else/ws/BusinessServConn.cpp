#include "BusinessServConn.h"
#include "MsgConn.h"
#include "IM.Other.pb.h"

static CHandlerMap* s_handler_map;

using namespace IM::BaseDefine;

static bool m_enable_business_async = false;

static ConnMap_t g_business_server_conn_map;

static serv_info_t* g_db_server_list = NULL;
static uint32_t		g_db_server_count = 0;			// 到DBServer的总连接数
static uint32_t		g_db_server_login_count = 0;	// 到进行登录处理的DBServer的总连接数

static void business_server_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	CBusinessServConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_business_server_conn_map.begin(); it != g_business_server_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (CBusinessServConn*)it_old->second;
		if (pConn->IsOpen()) {
			pConn->OnTimer(cur_time);
		}
	}

	// reconnect DB Storage Server
	// will reconnect in 4s, 8s, 16s, 32s, 64s, 4s 8s ...
	serv_check_reconnect<CBusinessServConn>(g_db_server_list, g_db_server_count);
}

void init_business_serv_conn(serv_info_t* server_list, uint32_t server_count, uint32_t concur_conn_cnt)
{
	g_db_server_list = server_list;
	g_db_server_count = server_count;

	uint32_t total_db_instance = server_count / concur_conn_cnt;
	g_db_server_login_count = (total_db_instance / 2) * concur_conn_cnt;
	DEBUG_I("DB server connection index for login business: [0, %u), for other business: [%u, %u) ",
			g_db_server_login_count, g_db_server_login_count, g_db_server_count);

	serv_init<CBusinessServConn>(g_db_server_list, g_db_server_count);

	netlib_register_timer(business_server_conn_timer_callback, NULL, 1000);
}

// get a random db server connection in the range [start_pos, stop_pos)
static CBusinessServConn* get_business_server_conn_in_range(uint32_t start_pos, uint32_t stop_pos)
{
	uint32_t i = 0;
	CBusinessServConn* pDbConn = NULL;

	// determine if there is a valid DB server connection
	for (i = start_pos; i < stop_pos; i++) {
		pDbConn = (CBusinessServConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	// no valid DB server connection
	if (i == stop_pos) {
		return NULL;
	}

	// return a random valid DB server connection
	while (true) {
		int i = rand() % (stop_pos - start_pos) + start_pos;
		pDbConn = (CBusinessServConn*)g_db_server_list[i].serv_conn;
		if (pDbConn && pDbConn->IsOpen()) {
			break;
		}
	}

	return pDbConn;
}

CBusinessServConn* get_business_serv_conn_for_login()
{
	// 先获取login业务的实例，没有就去获取其他业务流程的实例
	CBusinessServConn* pDBConn = get_business_server_conn_in_range(0, g_db_server_login_count);
	if (!pDBConn) {
		pDBConn = get_business_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	}

    if(pDBConn == NULL){
        pDBConn = new CBusinessServConn();
    }
	return pDBConn;
}

CBusinessServConn* get_business_serv_conn()
{
	// 先获取其他业务流程的实例，没有就去获取login业务的实例
	CBusinessServConn* pDBConn = get_business_server_conn_in_range(g_db_server_login_count, g_db_server_count);
	if (!pDBConn) {
		pDBConn = get_business_server_conn_in_range(0, g_db_server_login_count);
	}

    if(pDBConn == NULL){
        pDBConn = new CBusinessServConn();
    }
	return pDBConn;
}

CBusinessServConn::CBusinessServConn()
{
	m_bOpen = false;
    s_handler_map = CHandlerMap::getInstance();
}

CBusinessServConn::~CBusinessServConn()
{

}

int CBusinessServConn::SendPdu(CImPdu* pPdu) {
    if(m_enable_business_async){
        return Send(pPdu->GetBuffer(), (int)pPdu->GetLength());
    }else{

    }
}

void CBusinessServConn::Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx)
{
	DEBUG_I("Connecting to DB Storage Server %s:%d ", server_ip, server_port);

	m_serv_idx = serv_idx;
	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_business_server_conn_map);

	if (m_handle != NETLIB_INVALID_HANDLE) {
//        DEBUG_I("===>> m_handle: %d,serv_idx: %d",m_handle,serv_idx);
        g_business_server_conn_map.insert(make_pair(m_handle, this));
	}
}

void CBusinessServConn::Close()
{
	// reset server information for the next connect
	serv_reset<CBusinessServConn>(g_db_server_list, g_db_server_count, m_serv_idx);

	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_business_server_conn_map.erase(m_handle);
	}

	ReleaseRef();
}

void CBusinessServConn::OnConfirm()
{
	DEBUG_I("connect to db server success");
	m_bOpen = true;
	g_db_server_list[m_serv_idx].reconnect_cnt = MIN_RECONNECT_CNT / 2;
}

void CBusinessServConn::OnClose()
{
	DEBUG_I("onclose from db server handle=%d", m_handle);
	Close();
}

void CBusinessServConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + SERVER_HEARTBEAT_INTERVAL) {
        IM::Other::IMHeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_HEARTBEAT);
		SendPdu(&pdu);
	}

	if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT) {
		DEBUG_I("conn to db server timeout");
		Close();
	}
}

void CBusinessServConn::HandlePdu(CImPdu* pPdu)
{
    pdu_handler_callbakc_t handler = s_handler_map->GetHandlerCallback(pPdu->GetCommandId());
    if (handler) {
        handler(pPdu);
    } else {
        DEBUG_I("CBusinessServConn no handler for packet type: %d", pPdu->GetCommandId());
    }

}

//void CBusinessServConn::_HandleStopReceivePacket(CImPdu* pPdu)
//{
//	DEBUG_I("HandleStopReceivePacket, from %s:%d.",
//			g_db_server_list[m_serv_idx].server_ip.c_str(), g_db_server_list[m_serv_idx].server_port);
//
//	m_bOpen = false;
//}
//

//void CBusinessServConn::_HandleGetDeviceTokenResponse(CImPdu *pPdu)
//{
//    IM::Server::IMGetDeviceTokenRsp msg;
//    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
//
//    IM::Message::IMMsgData msg2;
//    CHECK_PB_PARSE_MSG(msg2.ParseFromArray(msg.attach_data().c_str(), msg.attach_data().length()));
//    string msg_data = msg2.msg_data();
//    uint32_t msg_type = msg2.msg_type();
//    uint32_t from_id = msg2.from_user_id();
//    uint32_t to_id = msg2.to_session_id();
//    if (msg_type == IM::BaseDefine::MSG_TYPE_SINGLE_TEXT || msg_type == IM::BaseDefine::MSG_TYPE_GROUP_TEXT)
//    {
////        char* msg_out = NULL;
////        uint32_t msg_out_len = 0;
////        msg_data = string(msg_out, msg_out_len);
////
////        if (pAes->Decrypt(msg_data.c_str(), msg_data.length(), &msg_out, msg_out_len) == 0)
////        {
////            msg_data = string(msg_out, msg_out_len);
////        }
////        else
////        {
////            DEBUG_I("HandleGetDeviceTokenResponse, decrypt msg failed, from_id: %u, to_id: %u, msg_type: %u.", from_id, to_id, msg_type);
////            return;
////        }
////        pAes->Free(msg_out);
//    }
//
//    build_ios_push_flash(msg_data, msg2.msg_type(), from_id);
//    //{
//    //    "msg_type": 1,
//    //    "from_id": "1345232",
//    //    "group_type": "12353",
//    //}
//    jsonxx::Object json_obj;
//    json_obj << "msg_type" << (uint32_t)msg2.msg_type();
//    json_obj << "from_id" << from_id;
//    if (CHECK_MSG_TYPE_GROUP(msg2.msg_type())) {
//        json_obj << "group_id" << to_id;
//    }
//
//    uint32_t user_token_cnt = msg.user_token_info_size();
//    DEBUG_I("HandleGetDeviceTokenResponse, user_token_cnt = %u.", user_token_cnt);
//
//    IM::Server::IMPushToUserReq msg3;
//    for (uint32_t i = 0; i < user_token_cnt; i++)
//    {
//        IM::BaseDefine::UserTokenInfo user_token = msg.user_token_info(i);
//        uint32_t user_id = user_token.user_id();
//        string device_token = user_token.token();
//        uint32_t push_cnt = user_token.push_count();
//        uint32_t client_type = user_token.user_type();
//        //自己发得消息不给自己发推送
//        if (from_id == user_id) {
//            continue;
//        }
//
//        DEBUG_I("HandleGetDeviceTokenResponse, user_id = %u, device_token = %s, push_cnt = %u, client_type = %u.",
//            user_id, device_token.c_str(), push_cnt, client_type);
//
//        CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(user_id);
//        if (pUser)
//        {
//            msg3.set_flash(msg_data);
//            msg3.set_data(json_obj.json());
//            IM::BaseDefine::UserTokenInfo* user_token_tmp = msg3.add_user_token_list();
//            user_token_tmp->set_user_id(user_id);
//            user_token_tmp->set_user_type((IM::BaseDefine::ClientType)client_type);
//            user_token_tmp->set_token(device_token);
//            user_token_tmp->set_push_count(push_cnt);
//            //pc client登录，则为勿打扰式推送
//            if (pUser->GetPCLoginStatus() == IM_PC_LOGIN_STATUS_ON)
//            {
//                user_token_tmp->set_push_type(IM_PUSH_TYPE_SILENT);
//                DEBUG_I("HandleGetDeviceTokenResponse, user id: %d, push type: silent.", user_id);
//            }
//            else
//            {
//                user_token_tmp->set_push_type(IM_PUSH_TYPE_NORMAL);
//                DEBUG_I("HandleGetDeviceTokenResponse, user id: %d, push type: normal.", user_id);
//            }
//        }
//        else
//        {
//            IM::Server::IMPushToUserReq msg4;
//            msg4.set_flash(msg_data);
//            msg4.set_data(json_obj.json());
//            IM::BaseDefine::UserTokenInfo* user_token_tmp = msg4.add_user_token_list();
//            user_token_tmp->set_user_id(user_id);
//            user_token_tmp->set_user_type((IM::BaseDefine::ClientType)client_type);
//            user_token_tmp->set_token(device_token);
//            user_token_tmp->set_push_count(push_cnt);
//            user_token_tmp->set_push_type(IM_PUSH_TYPE_NORMAL);
//            ImPdu pdu;
//            pdu.SetPBMsg(&msg4);
//            pdu.SetServiceId(SID_OTHER);
//            pdu.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);
//
//            CPduAttachData attach_data(ATTACH_TYPE_PDU_FOR_PUSH, 0, pdu.GetBodyLength(), pdu.GetBodyData());
//            IM::Buddy::IMUsersStatReq msg5;
//            msg5.set_user_id(0);
//            msg5.add_user_id_list(user_id);
//            msg5.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
//            ImPdu pdu2;
//            pdu2.SetPBMsg(&msg5);
//            pdu2.SetServiceId(SID_BUDDY_LIST);
//            pdu2.SetCommandId(CID_BUDDY_LIST_USERS_STATUS_REQUEST);
//            CRouteServConn* route_conn = get_route_serv_conn();
//            if (route_conn)
//            {
//                route_conn->SendPdu(&pdu2);
//            }
//        }
//    }
//
//    if (msg3.user_token_list_size() > 0)
//    {
//        ImPdu pdu3;
//        pdu3.SetPBMsg(&msg3);
//        pdu3.SetServiceId(SID_OTHER);
//        pdu3.SetCommandId(CID_OTHER_PUSH_TO_USER_REQ);
//
//        CPushServConn* PushConn = get_push_serv_conn();
//        if (PushConn) {
//            PushConn->SendPdu(&pdu3);
//        }
//    }
//}
//

bool CBusinessServConn::isOnService(){
    return IsOpen() && m_enable_business_async;
}

void set_enable_business_async(bool flag){
    m_enable_business_async = flag;
}

#include "HttpConn.h"
#include "ptp_global/Helpers.h"
#include "json/json.h"
#include "PTP.Common.pb.h"
#include "PTP.File.pb.h"
#include <unistd.h>

using namespace PTP::File;
using namespace PTP::Common;

static HttpConnMap_t g_http_conn_map;

#define NETLIB_MAX_SOCKET_BUF_SIZE1		(2 * 1024 * 1024)

static uint32_t g_conn_handle_generator = 0;
CLock CHttpConn::s_list_lock;
list<Response_t*> CHttpConn::s_response_pdu_list;

CHttpConn* FindHttpConnByHandle(uint32_t conn_handle)
{
    CHttpConn* pConn = NULL;
    HttpConnMap_t::iterator it = g_http_conn_map.find(conn_handle);
    if (it != g_http_conn_map.end())
    {
        pConn = it->second;
    }

    return pConn;
}

void httpconn_callback(void* callback_data, uint8_t msg, uint32_t handle,
        uint32_t uParam, void* pParam)
{
    NOTUSED_ARG(uParam);
    NOTUSED_ARG(pParam);

    // convert void* to uint32_t, oops
    uint32_t conn_handle = *((uint32_t*) (&callback_data));
    CHttpConn* pConn = FindHttpConnByHandle(conn_handle);
    if (!pConn)
    {
        return;
    }

    switch (msg)
    {
    case NETLIB_MSG_READ:
        pConn->OnRead();
        break;
    case NETLIB_MSG_WRITE:
        pConn->OnWrite();
        break;
    case NETLIB_MSG_CLOSE:
        pConn->OnClose();
        break;
    default:
        DEBUG_D("!!!httpconn_callback error msg: %d", msg);
        break;
    }
}

void http_conn_loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    CHttpConn::SendResponsePduList();
}

void http_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle,
        void* pParam)
{
    CHttpConn* pConn = NULL;
    HttpConnMap_t::iterator it, it_old;
    uint64_t cur_time = get_tick_count();

    for (it = g_http_conn_map.begin(); it != g_http_conn_map.end();)
    {
        it_old = it;
        it++;

        pConn = it_old->second;
        pConn->OnTimer(cur_time);
    }
}

void init_http_conn()
{
    netlib_register_timer(http_conn_timer_callback, NULL, 1000);
    netlib_add_loop(http_conn_loop_callback, NULL);
}

CHttpTask::CHttpTask(Request_t request)
{
    m_ConnHandle = request.conn_handle;
    m_nMethod = request.method;
    m_isPdu = request.isPdu;
    m_strUrl = request.strUrl;
    m_strContentType = request.strContentType;
    m_pContent = request.pContent;
    m_nContentLen = request.nContentLen;
    m_strAccessHost = request.strAccessHost;
}

CHttpTask::~CHttpTask()
{
}

void CHttpTask::run()
{
    if(HTTP_GET == m_nMethod){
        OnGet();
    } else if(HTTP_POST == m_nMethod){
       OnPost();
    }else{
        char* pContent = new char[strlen(HTTP_RESPONSE_403)];
        snprintf(pContent, strlen(HTTP_RESPONSE_403), HTTP_RESPONSE_403);
        CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
    }
}

void CHttpTask::OnPost(){
    while (true){
        uint32_t pduLen = CByteStream::ReadUint32(reinterpret_cast<uchar_t *>(m_pContent));
        if(pduLen <= m_nContentLen){
            DEBUG_D("m_conn_handle=%d,pduLen=%d",m_ConnHandle,pduLen);
            HandlePduBuf(reinterpret_cast<uchar_t *>(m_pContent),pduLen);
        }
        break;
    }
}

void CHttpTask::FileImgDownloadReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
    FileImgDownloadReq msg;
    FileImgDownloadRes msg_rsp;
    for(;;){
        msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
        uint32_t  nFileSize = 0;
        int32_t nTmpSize = 0;
        string strPath;
        ERR error = NO_ERROR;
        char *pContent = NULL;
        char* pFileContent;
        uint32_t pContentLen = 0;
        string strUrl = msg.file_path();
        char szResponseHeader[1024];
        if(g_fileManager->getAbsPathByUrl(strUrl, strPath ) == 0){
            nTmpSize = (uint32_t)File::getFileSize((char*)strPath.c_str());
            if(nTmpSize != -1)
            {
                pFileContent = new char[nTmpSize];
                g_fileManager->downloadFileByUrl((char*)strUrl.c_str(), pFileContent, &nFileSize);
                msg_rsp.set_file_data(pFileContent,nFileSize);
            }else{
                error = E_SERVER_NOT_FOUND;
            }
        }else{
            error = E_SYSTEM;
        }
        if(m_isPdu){
            msg_rsp.set_error(error);
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_FILE);
            pdu.SetCommandId(CID_FileImgDownloadRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            auto httpBody1 = msg_rsp.SerializeAsString();
            uint32_t content_length = msg_rsp.ByteSizeLong();
            auto httpBody = httpBody1.data();
            pContentLen = pdu.GetLength();
            pContent = new char[pContentLen];
            memcpy(pContent,pdu.GetBuffer(),pContentLen);
        }else{
            if(error > NO_ERROR){
                if(error == E_SERVER_NOT_FOUND){
                    pContentLen = (uint32_t)strlen(HTTP_RESPONSE_404);
                    pContent = new char[pContentLen];
                    snprintf(pContent, pContentLen, HTTP_RESPONSE_404);
                }
                if(error == E_SYSTEM){
                    pContentLen = (uint32_t)strlen(HTTP_RESPONSE_500);
                    pContent = new char[pContentLen];
                    snprintf(pContent, pContentLen, HTTP_RESPONSE_500);
                }
            }else{
                snprintf(szResponseHeader,sizeof(szResponseHeader), HTTP_RESPONSE_EXTEND, nTmpSize);
                auto nLen = strlen(szResponseHeader);
                pContent = new char[nLen + nFileSize];
                memcpy(pContent, szResponseHeader, nLen);
                memcpy(pContent, pFileContent, nFileSize);
                pContentLen = nFileSize + nLen;
            }
        }
        CHttpConn::AddResponsePdu(conn_uuid, pContent, pContentLen);
        break;
    }
}

void CHttpTask::FileImgUploadReqCmd(CImPdu* pPdu, uint32_t conn_uuid){
    FileImgUploadReq msg;
    for(;;){
        msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
        const string file_group = "media";
        const auto& file_data = msg.file_data();
        auto file_part = msg.file_part();
        auto file_total_parts = msg.file_total_parts();
        string suffix = "0_" + to_string(file_part) + "_" + to_string(file_total_parts) + ".dat";
        char filePath[100] ={ 0 };
        g_fileManager->uploadFile(suffix.data(), file_data.data(), msg.file_data().size(), filePath);
        FileImgUploadRes msg_rsp;
        msg_rsp.set_file_path(filePath);
        msg_rsp.set_error(NO_ERROR);

        char *pContent = NULL;

        uint32_t pContentLen = 0;
        if(m_isPdu){
            CImPdu pdu;
            pdu.SetPBMsg(&msg_rsp);
            pdu.SetServiceId(S_FILE);
            pdu.SetCommandId(CID_FileImgUploadRes);
            pdu.SetSeqNum(pPdu->GetSeqNum());
            auto httpBody1 = msg_rsp.SerializeAsString();
            uint32_t content_length = msg_rsp.ByteSizeLong();
            auto httpBody = httpBody1.data();
            pContentLen = pdu.GetLength();
            pContent = new char[pContentLen];
            memcpy(pContent,pdu.GetBuffer(),pContentLen);
        }else{
            Json::Value http_body;
            http_body["error"] = msg_rsp.error();
            http_body["file_path"] = msg_rsp.file_path();
            string http_body_str = http_body.toStyledString();
            uint32_t content_len = http_body_str.size();
            char headerBuf[1024] = {};
            strcat(headerBuf, "HTTP/1.1 200 OK\r\n");
            strcat(headerBuf, "Connection:close\r\n");
            strcat(headerBuf, "Access-Control-Allow-Origin: *\r\n");
            strcat(headerBuf, ("Content-Length: " + to_string(content_len) + "\r\n\r\n").c_str());
            pContentLen = content_len + strlen( headerBuf);
            pContent = new char[pContentLen]();
            strcat(pContent, headerBuf);
            strcat(pContent, http_body_str.c_str());
        }

        CHttpConn::AddResponsePdu(conn_uuid, pContent, pContentLen);
        break;
    }
}
void  CHttpTask::OnGet()
{
        uint32_t  nFileSize = 0;
        int32_t nTmpSize = 0;
        string strPath;
        if(g_fileManager->getAbsPathByUrl(m_strUrl, strPath ) == 0)
        {
            nTmpSize = File::getFileSize((char*)strPath.c_str());
            if(nTmpSize != -1)
            {
                char szResponseHeader[1024];
                size_t nPos = strPath.find_last_of(".");
                string strType = strPath.substr(nPos + 1, strPath.length() - nPos);
                if(strType == "jpg" || strType == "JPG" || strType == "jpeg" || strType == "JPEG" || strType == "png" || strType == "PNG" || strType == "gif" || strType == "GIF")
                {
                    snprintf(szResponseHeader, sizeof(szResponseHeader), HTTP_RESPONSE_IMAGE, nTmpSize, strType.c_str());
                }
                else
                {
                    snprintf(szResponseHeader,sizeof(szResponseHeader), HTTP_RESPONSE_EXTEND, nTmpSize);
                }
                int nLen = strlen(szResponseHeader);
                char* pContent = new char[nLen + nTmpSize];
                memcpy(pContent, szResponseHeader, nLen);
                g_fileManager->downloadFileByUrl((char*)m_strUrl.c_str(), pContent + nLen, &nFileSize);
                int nTotalLen = nLen + nFileSize;
                CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
            }
            else
            {
                int nTotalLen = strlen(HTTP_RESPONSE_404);
                char* pContent = new char[nTotalLen];
                snprintf(pContent, nTotalLen, HTTP_RESPONSE_404);
                CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
                DEBUG_D("File size is invalied\n");
                
            }
        }
        else
        {
        	int nTotalLen = strlen(HTTP_RESPONSE_500);
			char* pContent = new char[nTotalLen];
			snprintf(pContent, nTotalLen, HTTP_RESPONSE_500);
			CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
        }
}

void CHttpTask::HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len)
{
    CImPdu* pPdu = NULL;
    try
    {
        while (true){
            pPdu = CImPdu::ReadPdu(pdu_buf, pdu_len);
            if (pPdu->GetCommandId() == PTP::Common::CID_FileImgUploadReq) {
                FileImgUploadReqCmd(pPdu,m_ConnHandle);
            }else if(pPdu->GetCommandId() == PTP::Common::CID_FileImgDownloadReq){
                FileImgDownloadReqCmd(pPdu,m_ConnHandle);
            }
            break;
        }
    } catch (CPduException& ex) {
        DEBUG_E("!!!catch exception, sid=%u, cid=%u, err_code=%u, err_msg=%s, close the connection ",
                  ex.GetServiceId(), ex.GetCommandId(), ex.GetErrorCode(), ex.GetErrorMsg());
    }

    if (pPdu) {
        delete pPdu;
        pPdu = NULL;
    }
}

CHttpConn::CHttpConn()
{
    m_busy = false;
    m_sock_handle = NETLIB_INVALID_HANDLE;
    m_state = CONN_STATE_IDLE;

    m_last_send_tick = m_last_recv_tick = get_tick_count();
    m_conn_handle = ++g_conn_handle_generator;
    if (m_conn_handle == 0)
    {
        m_conn_handle = ++g_conn_handle_generator;
    }
}

CHttpConn::~CHttpConn()
{
}

int CHttpConn::Send(void* data, int len)
{
    m_last_send_tick = get_tick_count();

    if (m_busy)
    {
        m_out_buf.Write(data, len);
        return len;
    }
    int offset = 0;
    int remain = len;
    while (remain > 0) {
        int send_size = remain;
        if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE1) {
            send_size = NETLIB_MAX_SOCKET_BUF_SIZE1;
        }
        int ret = netlib_send(m_sock_handle, (char*)data + offset , send_size);
        DEBUG_D("Send: %d, %d / %d, remain:%d, send_size:%d",ret,offset,len,remain,send_size);
        if (ret <= 0) {
            ret = 0;
            break;
        }

        offset += ret;
        remain -= ret;
    }

    if (remain > 0)
    {
        m_out_buf.Write((char*)data + offset, remain);
        m_busy = true;
        DEBUG_D("send busy, remain=%d ", m_out_buf.GetWriteOffset());
    }
    else
    {
        OnSendComplete();
    }
    return len;
}

void CHttpConn::Close()
{
    m_state = CONN_STATE_CLOSED;

    g_http_conn_map.erase(m_conn_handle);
    netlib_close(m_sock_handle);

    ReleaseRef();
}

void CHttpConn::OnConnect(net_handle_t handle)
{
    DEBUG_D("OnConnect, handle=%d", handle);
    m_sock_handle = handle;
    m_state = CONN_STATE_CONNECTED;
    g_http_conn_map.insert(make_pair(m_conn_handle, this));

    netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*) httpconn_callback);
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA,
            reinterpret_cast<void *>(m_conn_handle));
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*) &m_peer_ip);
}

void CHttpConn::OnRead()
{
    for (;;)
    {
        uint32_t free_buf_len = m_in_buf.GetAllocSize()
                - m_in_buf.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE + 1)
            m_in_buf.Extend(READ_BUF_SIZE + 1);
        int ret = netlib_recv(m_sock_handle,
                m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(),
                READ_BUF_SIZE);
        DEBUG_D("m_conn_handle=%d,ret=%d,offset=%d,READ_BUF_SIZE=%d",m_conn_handle,ret,m_in_buf.GetWriteOffset(),READ_BUF_SIZE);
        if (ret <= 0)
            break;
        m_in_buf.IncWriteOffset(ret);
        m_last_recv_tick = get_tick_count();
    }

    char* in_buf = (char*) m_in_buf.GetBuffer();
    uint32_t buf_len = m_in_buf.GetWriteOffset();
    in_buf[buf_len] = '\0';
    char method[5];
    memcpy( method, in_buf, 4 );
    method[4] = '\0';
    if(string(method) == "POST"){
        m_HttpParser.ParseHttpContent(in_buf, buf_len);
        if (m_HttpParser.IsReadAll())
        {
            string strUrl = m_HttpParser.GetUrl();
            if (strUrl.find("..") != strUrl.npos) {
                Close();
                return;
            }
            m_access_host = m_HttpParser.GetHost();
            if (m_HttpParser.GetContentLen() > HTTP_UPLOAD_MAX)
            {
                char url[128];
                char pContent[1024];
                snprintf(pContent, sizeof(pContent), HTTP_RESPONSE_500);
                Send(pContent, strlen(pContent));
                return;
            }

            int nContentLen = m_HttpParser.GetContentLen();
            char* pContent = NULL;
            if(nContentLen != 0)
            {
                try {
                    pContent =new char[nContentLen];
                    memcpy(pContent, m_HttpParser.GetBodyContent(), nContentLen);
                }
                catch(...)
                {
                    DEBUG_D("not enough memory");
                    char szResponse[HTTP_RESPONSE_500_LEN + 1];
                    snprintf(szResponse, HTTP_RESPONSE_500_LEN, "%s", HTTP_RESPONSE_500);
                    Send(szResponse, HTTP_RESPONSE_500_LEN);
                    return;
                }
            }
            Request_t request;
            request.conn_handle = m_conn_handle;
            request.isPdu = false;
            request.method = m_HttpParser.GetMethod();;
            request.nContentLen = nContentLen;
            request.pContent = pContent;
            request.strAccessHost = m_HttpParser.GetHost();
            request.strContentType = m_HttpParser.GetContentType();
            request.strUrl = m_HttpParser.GetUrl() + 1;
            CHttpTask* pTask = new CHttpTask(request);
            if(HTTP_GET == m_HttpParser.GetMethod()){
                g_GetThreadPool.AddTask(pTask);
            }else{
                g_PostThreadPool.AddTask(pTask);
            }
        }
    }else{
        uint32_t pduLen = CByteStream::ReadUint32(m_in_buf.GetBuffer());
        if(pduLen <= buf_len){
            DEBUG_D("m_conn_handle=%d,pduLen=%d",m_conn_handle,pduLen);
            Request_t request;
            request.conn_handle = m_conn_handle;
            request.isPdu = true;
            request.method = HTTP_POST;
            request.nContentLen = (int)pduLen;
            request.pContent = (char *) m_in_buf.GetBuffer();
            request.strAccessHost = m_HttpParser.GetHost();
            request.strContentType = m_HttpParser.GetContentType();
            request.strUrl = m_HttpParser.GetUrl() + 1;
            CHttpTask* pTask = new CHttpTask(request);
            g_PostThreadPool.AddTask(pTask);
        }
    }
    DEBUG_D("m_conn_handle=%d,buf_len=%d",m_conn_handle,buf_len);

}

void CHttpConn::OnWrite()
{
    DEBUG_D("OnWrite m_busy: %b",m_busy);
    if (!m_busy)
        return;

    int ret = netlib_send(m_sock_handle, m_out_buf.GetBuffer(),
            m_out_buf.GetWriteOffset());

    DEBUG_D("OnWrite ret: %d",ret);
    if (ret < 0)
        ret = 0;

    int out_buf_size = (int) m_out_buf.GetWriteOffset();

    m_out_buf.Read(NULL, ret);

    if (ret < out_buf_size)
    {
        m_busy = true;
        DEBUG_D("not send all, remain=%d", m_out_buf.GetWriteOffset());
        OnWrite();
    } else
    {
        m_busy = false;
        OnSendComplete();
    }
}

void CHttpConn::OnClose()
{
    Close();
}

void CHttpConn::OnTimer(uint64_t curr_tick)
{
    if (curr_tick > m_last_recv_tick + HTTP_CONN_TIMEOUT)
    {
        DEBUG_D("HttpConn timeout, handle=%d", m_conn_handle);
        Close();
    }
}

void CHttpConn::AddResponsePdu(uint32_t conn_handle, char* pContent, int nLen)
{
    Response_t* pResp = new Response_t;
    pResp->conn_handle = conn_handle;
    pResp->pContent = pContent;
    pResp->content_len = nLen;

    s_list_lock.lock();
    s_response_pdu_list.push_back(pResp);
    s_list_lock.unlock();
}

void CHttpConn::SendResponsePduList()
{
    s_list_lock.lock();
    while (!s_response_pdu_list.empty()) {
        Response_t* pResp = s_response_pdu_list.front();
        s_response_pdu_list.pop_front();
        s_list_lock.unlock();

        CHttpConn* pConn = FindHttpConnByHandle(pResp->conn_handle);
        if (pConn) {
            pConn->Send(pResp->pContent, pResp->content_len);
        }
        if(pResp->pContent != NULL)
        {
            delete [] pResp->pContent;
            pResp->pContent = NULL;
        }
        if(pResp != NULL)
        {
            delete pResp;
            pResp = NULL;
        }

        if(pConn != NULL)
        {
            pConn->Close();
        }


        s_list_lock.lock();
    }

    s_list_lock.unlock();
}

void CHttpConn::OnSendComplete()
{
    //Close();
}


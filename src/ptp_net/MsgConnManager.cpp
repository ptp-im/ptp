#include "ptp_net/MsgConnManager.h"
#include "ptp_net/AccountManager.h"
#include "ptp_net/MsgConn.h"
#include "ptp_toolbox/data/bytes_array.h"
#include "ptp_common/socket_client.h"

#ifdef ANDROID
#include <jni.h>
#include "../../tgnet/FileLog.h"
#include "../../tgnet/Defines.h"
#include "../../tgnet/ByteArray.h"
#include "../../tgnet/NativeByteBuffer.h"
#include "../../tgnet/BuffersStorage.h"
#else
#include "ptp_common/Logger.h"
#include "ptp_net/Defines.h"
#include "ptp_net/ByteArray.h"
#include "ptp_net/NativeByteBuffer.h"
#include "ptp_net/BuffersStorage.h"
#endif

static ConnectiosManagerDelegate* m_delegate;

size_t write_data_string(void *ptr, size_t size, size_t nmemb, void *userp)
{
    size_t len = size * nmemb;
    string* response = (string *)userp;
    response->append((char*)ptr, len);
    return len;
}

static bool done = false;
static uint32_t		process_task_cnt = 0;
static bool		    processing = false;
static uint32_t		process_upload_cnt = 0;
static bool		    processingUpload = false;
std::queue<NativeByteBuffer *> taskList;
std::queue<NativeByteBuffer *> uploadList;

static uint32_t getStringSize(string &str){
    auto *sizeCalculator = new NativeByteBuffer(true);
    sizeCalculator->clearCapacity();
    sizeCalculator->writeString(str);
    return sizeCalculator->capacity();
}

static void handleUpload(NativeByteBuffer *task){
    int32_t len = task->readInt32(nullptr);
    int32_t type = task->readInt32(nullptr);
    int32_t seqNum = task->readInt32(nullptr);
    int32_t accountId = task->readInt32(nullptr);
    DEBUG_D("process_task type=%d",type);
    uint32_t notifySize = 0;
    NativeByteBuffer *buffer;
    len = len - NativeInvokeHeaderSize;
    ByteArray* pduByteArray = task->readBytes(len,nullptr);
    uint8_t *pduBytes = pduByteArray->bytes;

    while (true){
        unsigned char responseBody[ SOCKET_BUF_SIZE ];
        uint32_t responseBodyLen;
        string ip = MSFS_SERVER_IP_1;
        uint16_t port = MSFS_SERVER_PORT_1;
        socketSend(ip.c_str(),port,pduBytes,len,responseBody,responseBodyLen);
        DEBUG_D("%s", ptp_toolbox::data::bytes_to_hex(responseBody,responseBodyLen).c_str());
        CImPdu *pPdu;
        pPdu = CImPdu::ReadPdu(responseBody, responseBodyLen);
        DEBUG_D("cid:%d",pPdu->GetCommandId());
        DEBUG_D("sno:%d",pPdu->GetSeqNum());
        buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize + responseBodyLen );
        buffer->writeInt32(NativeInvokeHeaderSize + responseBodyLen);
        buffer->writeInt32(NativeInvokeType_PDU_RECV);
        buffer->writeInt32(seqNum);
        buffer->writeInt32(accountId);
        buffer->writeBytes(pPdu->GetBuffer(),responseBodyLen);
        buffer->position(0);
        buffer->reuse();
        MsgConnManager::onNotify(buffer);
        break;
    }

}

static void handleTask(NativeByteBuffer *task){
    int32_t len = task->readInt32(nullptr);
    int32_t type = task->readInt32(nullptr);
    int32_t seqNum = task->readInt32(nullptr);
    int32_t accountId = task->readInt32(nullptr);
    DEBUG_D("process_task type=%d",type);
    uint32_t notifySize = 0;
    NativeByteBuffer *buffer;
    switch (type) {
        case NativeInvokeType_HEART_BEAT:
        {
            buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            break;
        }
        case NativeInvokeType_SIGN_GROUP_MSG:
        {
            len = len - NativeInvokeHeaderSize;
            int32_t groupIdx = task->readInt32(nullptr);
            ByteArray* captchaByteArray = task->readBytes(len - 4,nullptr);
            uint8_t *captchaBytes = captchaByteArray->bytes;
            string captcha = string((char *)captchaBytes, len - 4);
            DEBUG_D("NativeInvokeType_SIGN_GROUP_MSG len=%d,captcha=%s,groupIdx=%d",len,captcha.c_str(),groupIdx);
            string sign = AccountManager::getInstance(accountId).signGroupMessage(captcha,groupIdx);
            auto* signBytes = (uchar_t *) sign.data();
            notifySize = NativeInvokeHeaderSize + sign.size();

            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeBytes(signBytes,sign.size());
            break;
        }
        case NativeInvokeType_PDU_DOWNLOAD:
        case NativeInvokeType_PDU_UPLOAD:
        {
            len = len - NativeInvokeHeaderSize;
            ByteArray* pduByteArray = task->readBytes(len,nullptr);
            uint8_t *pduBytes = pduByteArray->bytes;
            while (true){
                unsigned char responseBody[ SOCKET_BUF_SIZE ];
                uint32_t responseBodyLen;
                string ip = MSFS_SERVER_IP_1;
                uint16_t port = MSFS_SERVER_PORT_1;
                socketSend(ip.c_str(),port,pduBytes,len,responseBody,responseBodyLen);
                DEBUG_D("%s", ptp_toolbox::data::bytes_to_hex(responseBody,responseBodyLen).c_str());
                CImPdu *pPdu;
                pPdu = CImPdu::ReadPdu(responseBody, responseBodyLen);
                DEBUG_D("cid:%d",pPdu->GetCommandId());
                DEBUG_D("sno:%d",pPdu->GetSeqNum());
                buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize + responseBodyLen );
                buffer->writeInt32(NativeInvokeHeaderSize + responseBodyLen);
                buffer->writeInt32(NativeInvokeType_PDU_RECV);
                buffer->writeInt32(seqNum);
                buffer->writeInt32(accountId);
                buffer->writeBytes(pPdu->GetBuffer(),responseBodyLen);
                break;
            }
            break;
        }
        case NativeInvokeType_PDU_SEND:
        {
            len = len - NativeInvokeHeaderSize;
            ByteArray* pduByteArray = task->readBytes(len,nullptr);
            uint8_t *pduBytes = pduByteArray->bytes;
            AccountManager::getInstance(accountId).sendPdu(pduBytes,len);
            break;
        }
        case NativeInvokeType_SET_WORDS:
        {
            int32_t accountId1 = task->readInt32(nullptr);

            len = len - NativeInvokeHeaderSize - NativeInvokeIntSize;
            ByteArray* wordsByteArray = task->readBytes(len,nullptr);
            uint8_t *wordsBytes = wordsByteArray->bytes;
            string words1 = string((char *)wordsBytes, len);
            int res_code = 1;
            bool res_valid = AccountManager::validateWords(words1);
            if(res_valid){
                string address = AccountManager::getAddressByWords(words1);
                bool res = AccountManager::isAddressExists(address);
                if(!res){
                    bool res_setWords = AccountManager::getInstance(accountId1).setWords(words1);
                    if(res_setWords){
                        res_code = 0;
                    }else{
                        res_code = 3;
                    }
                }else{
                    res_code = 2;
                }
            }
            notifySize = NativeInvokeHeaderSize + NativeInvokeIntSize + NativeInvokeIntSize;
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeInt32(accountId1);
            buffer->writeInt32(res_code);
            break;
        }
        case NativeInvokeType_GET_WORDS:
        {
            int32_t accountId1 = task->readInt32(nullptr);
            string words = AccountManager::getInstance(accountId1).getWords();
            notifySize = NativeInvokeHeaderSize + getStringSize(words);
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeString(words);
            break;
        }
        case NativeInvokeType_GEN_WORDS:
        {
            string words = AccountManager::genWords();
            notifySize = NativeInvokeHeaderSize + getStringSize(words);
            buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(type);
            buffer->writeInt32(seqNum);
            buffer->writeInt32(accountId);
            buffer->writeString(words);
            break;
        }
        default:
            break;
    }

    if(notifySize > 0){
        buffer->position(0);
        buffer->reuse();
        MsgConnManager::onNotify(buffer);
    }
}


static void process_upload_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam){
    process_upload_cnt++;
    if(!uploadList.empty() && !processingUpload){
        processingUpload = true;
        DEBUG_D("process_upload size=%u",uploadList.size());
        while (true){
            if(uploadList.empty()){
                break;
            }
            auto it = uploadList.front();
            handleUpload(it);
            delete it;
            uploadList.pop();
        }
        processingUpload = false;
    }
    if(process_upload_cnt % 8 == 0){
//        NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(NativeInvokeHeaderSize );
//        buffer->writeInt32(NativeInvokeHeaderSize);
//        buffer->writeInt32(NativeInvokeType_HEART_BEAT);
//        buffer->writeInt32(0);
//        buffer->writeInt32(0);
//        buffer->position(0);
//        taskList.push(buffer->copy(NativeInvokeHeaderSize));
    }

    if(process_upload_cnt > 100000){
        process_upload_cnt = 0;
    }
}

static void process_task_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    process_task_cnt++;
    if(!taskList.empty() && !processing){
        processing = true;
        DEBUG_D("process_task size=%u",taskList.size());
        while (true){
            if(taskList.empty()){
                break;
            }
            auto it = taskList.front();
            handleTask(it);
            delete it;
            taskList.pop();
        }
        processing = false;
    }else{
        if(process_task_cnt % 100 == 0){
            //DEBUG_D("process_task size=%u",taskList.size());
        }
    }

    if(process_task_cnt > 100000){
        process_task_cnt = 0;
    }
}


void MsgConnManager::resetMsgConn(){
    DEBUG_D("resetMsgConn");
    reset_msg_conn();
}

void MsgConnManager::initMsgConn(uint32_t accountId){
    if(!done){
        done = true;
        DEBUG_D("initMsgConn accountId:%d", accountId);
        init_msg_conn(accountId);
        netlib_register_timer(process_task_timer_callback, NULL, 100);
        //netlib_register_timer(process_upload_timer_callback, NULL, 1000);
        netlib_eventloop();
    }else{
        DEBUG_D("initMsgConn accountId:%d done", accountId);
    }
}

void MsgConnManager::close(uint32_t accountId){
    DEBUG_D("close accountId:%d", accountId);
    close_msg_conn(accountId);
}


void MsgConnManager::invoke(NativeByteBuffer *request) {
    int32_t len = request->readInt32(nullptr);
    request->position(0);
    taskList.push(request->copy(len));
}

void MsgConnManager::setDelegate(ConnectiosManagerDelegate* delegate)
{
    m_delegate = delegate;
}

void MsgConnManager::onNotify(NativeByteBuffer *buffer)
{
    if( nullptr != m_delegate){
        m_delegate->onNotify(buffer);
    }
}

void MsgConnManager::onConnectionStateChanged(ConnectionState state, uint32_t accountId)
{
    if( nullptr != m_delegate){
        m_delegate->onConnectionStateChanged(state,(int32_t)accountId);
        if(state == ConnectionStateConnecting){
            string address = AccountManager::getInstance(accountId).getAccountAddress();
            int32_t notifySize = NativeInvokeHeaderSize + getStringSize(address);
            NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(notifySize);
            buffer->writeInt32((int32_t) notifySize);
            buffer->writeInt32(NativeInvokeType_CHANGE_ACCOUNT);
            buffer->writeInt32(NativeInvokeDefaultSepNum);
            buffer->writeInt32((int32_t)accountId);
            buffer->writeString(address);
            buffer->position(0);
            buffer->reuse();
            m_delegate->onNotify(buffer);
        }
    }
}

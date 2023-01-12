#include <gtest/gtest.h>
#include <array>
#include <cstdio>
#include <cstring>
#include "curl/curl.h"
#include "ptp_protobuf/PTP.File.pb.h"
#include "ptp_protobuf/ImPdu.h"
#include "ptp_global/SocketClient.h"
#include "ptp_global/Helpers.h"
#include "ptp_crypto/secp256k1_helpers.h"
#include "ptp_global/Utils.h"
#include "ptp_global/Logger.h"

using namespace std;
using namespace PTP::Common;

size_t write_data_string(void *ptr, size_t size, size_t nmemb, void *userp)
{
    size_t len = size * nmemb;
    string* response = (string *)userp;

    response->append((char*)ptr, len);

    return len;
}

//size_t write_data_binary(void *ptr, size_t size, size_t nmemb, AudioMsgInfo* pAudio)
//{
//    size_t nLen = size * nmemb;
//    if(pAudio->data_len + nLen <= pAudio->fileSize + 4)
//    {
//        memcpy(pAudio->data + pAudio->data_len, ptr, nLen);
//        pAudio->data_len += nLen;
//    }
//    return nLen;
//}

//
//TEST(ptp_common,HttpDns) {
//    int err;
//    string ip;
//    err = resolveIpFromDomain("localhost", ip);
//    DEBUG_D("%s", ip.c_str());
//    ASSERT_EQ(err,0);
//    err = resolveIpFromDomain("127.0.0.1", ip);
//    DEBUG_D("%s", ip.c_str());
//    ASSERT_EQ(err,0);
//    err = resolveIpFromDomain("localhost1", ip);
//    DEBUG_D("%s", ip.c_str());
//    ASSERT_EQ(err,1);
//}

TEST(ptp_common,handleRequestUrl) {
    string url1 = "http://www.google.com:443/www/sss/";
    Uri u1 = Uri::Parse(url1);
    ASSERT_EQ(u1.Protocol,"http");
    ASSERT_EQ(u1.Host,"www.google.com");
    ASSERT_EQ(u1.Port,443);
}

//TEST(test_upload, curl) {
//    struct stat statbuff;
//
//    string path;
//    path = "/Users/jack/Downloads/dmg/binance.dmg";
//    path = "/Users/jack/projects/client/android_client/app/jni/ptp/src/tests/test_common_upload_test.jpg";
//    //path = "/Users/jack/projects/client/android_client/app/jni/ptp/build.sh";
//    //path = "/Users/jack/data/projects/ptp/bd-im/apps/bd_ws_server/bd_ws_server";
//
//    auto fileSize = get_file_size(path.c_str());
//    auto *fileBuf = new char[fileSize];
//    get_file_content(path.c_str(),fileBuf,fileSize);
//    //put_file_content((path+".jpg").c_str(),fileBuf,fileSize);
//    ASSERT_EQ(strlen(fileBuf) > 0, true);
//
//
//    PTP::File::FileImgUploadReq msg;
//    msg.set_file_id("1");
//    msg.set_file_data(fileBuf,fileSize);
//    msg.set_file_total_parts(1);
//    msg.set_file_part(1);
//
//    ImPdu pdu;
//    pdu.SetPBMsg(&msg);
//    pdu.SetServiceId(S_FILE);
//    pdu.SetCommandId(CID_FileImgUploadReq);
//    pdu.SetSeqNum(0);
//
//
//    string strUrl = "http://127.0.0.1:7841";
//    while (true){
//        CURL* curl = curl_easy_init();
//        struct curl_slist *headerlist = NULL;
//        headerlist = curl_slist_append(headerlist, "Content-Type: multipart/form-data");
//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
//        curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);    //enable verbose for easier tracing
//        string body = "";
//        body.append((char*)pdu.GetBuffer(), pdu.GetLength());    // image buffer
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
//        string strResp;
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_string);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResp);
//        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
//        CURLcode res = curl_easy_perform(curl);
//        curl_easy_cleanup(curl);
//
//        if (CURLE_OK != res) {
//            printf("curl_easy_perform failed, res=%d\n", res);
//            break;
//        }
//        printf("strResp: %s\n", strResp.c_str());
//
//        break;
//    }
//
//    ASSERT_EQ(1,1);
//}
//
//
//TEST(ptp_common,HTTP_POST) {
//    string path;
//    path = "/Users/jack/Downloads/dmg/binance.dmg";
//    //path = "/Users/jack/projects/client/android_client/app/jni/ptp/src/tests/test_common_upload_test.jpg";
//    path = "/Users/jack/projects/client/android_client/app/jni/ptp/build.sh";
//
//    auto fileSize = get_file_size(path.c_str());
//    auto *fileBuf = new char[fileSize];
//    get_file_content(path.c_str(),fileBuf,fileSize);
//    //put_file_content((path+".jpg").c_str(),fileBuf,fileSize);
//    ASSERT_EQ(strlen(fileBuf) > 0, true);
//
//    PTP::File::FileImgUploadReq msg;
//    msg.set_file_id("1");
//    msg.set_file_data(fileBuf,fileSize);
//    msg.set_file_total_parts(1);
//    msg.set_file_part(1);
//
//    ImPdu pdu;
//    pdu.SetPBMsg(&msg);
//    pdu.SetServiceId(S_FILE);
//    pdu.SetCommandId(CID_FileImgUploadReq);
//    pdu.SetSeqNum(0);
//
//    struct HttpRequest httpRequest1;
//    httpRequest1.responseBody = new unsigned char[1024];
//    httpRequest1.url = "http://localhost:7841";
//    httpRequest1.method = HTTP_POST;
//    printf("requestBodyLen:%d\n",pdu.GetLength());
//    httpPost("http://localhost:7841", &httpRequest1, pdu.GetBuffer(), pdu.GetLength());
//    printf("responseBody\n\n%s\n",httpRequest1.responseBody);
//    ASSERT_EQ(httpRequest1.responseBody != NULL, true);
//}
////
////TEST(ptp_common,socket_send) {
////    string path;
////    path = "/Users/jack/Downloads/dmg/binance.dmg";
////    path = "/Users/jack/projects/client/android_client/app/jni/ptp/build.sh";
////
////    auto fileSize = get_file_size(path.c_str());
////    auto *fileBuf = new char[fileSize];
////    get_file_content(path.c_str(),fileBuf,fileSize);
////    //put_file_content((path+".jpg").c_str(),fileBuf,fileSize);
////    ASSERT_EQ(strlen(fileBuf) > 0, true);
////
////    PTP::File::FileImgUploadReq msg;
////    msg.set_file_id("1");
////    msg.set_file_data(fileBuf,fileSize);
////    msg.set_file_total_parts(1);
////    msg.set_file_part(1);
////
////    ImPdu pdu;
////    pdu.SetPBMsg(&msg);
////    pdu.SetServiceId(S_FILE);
////    pdu.SetCommandId(CID_FileImgUploadReq);
////    pdu.SetSeqNum(0);
////
////    printf("requestBodyLen:%d\n",pdu.GetLength());
////    unsigned char responseBody[1024];
////    uint32_t responseBodyLen;
////    string ip = "127.0.0.1";
////    uint16_t port = 7881;
////    port = 7841;
////    int i = 0;
////    for(;;){
////        i++;
////        if(i> 1){
////            break;
////        }
////        printf("===>>>>");
////        socketSend(ip.c_str(),port,pdu.GetBuffer(),pdu.GetLength(),responseBody,responseBodyLen);
////        ASSERT_EQ(responseBodyLen> 0, true);
////        //printf("%s\n",bytes_to_hex_string(responseBody,responseBodyLen).c_str());
////        CImPdu *pPdu;
////        pPdu = CImPdu::ReadPdu(responseBody, responseBodyLen);
////        printf("len:%d\n",pPdu->GetLength());
////        printf("cid:%d\n",pPdu->GetCommandId());
////        PTP::File::FileImgUploadRes msg_rsp;
////        msg_rsp.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
////        printf("file_path:%s\n",msg_rsp.file_path().c_str());
////
////        ASSERT_EQ(pPdu->GetCommandId(), CID_FileImgUploadRes);
////    }
////
////}
////
////
////TEST(ptp_common,socket_download) {
////    PTP::File::FileImgDownloadReq msg;
////    msg.set_file_path("g0/000/000/1672598092023246_123145375657_0_1_1.dat");
////
////    ImPdu pdu;
////    pdu.SetPBMsg(&msg);
////    pdu.SetServiceId(S_FILE);
////    pdu.SetCommandId(CID_FileImgDownloadReq);
////    pdu.SetSeqNum(1);
////
////    printf("requestBodyLen:%d\n",pdu.GetLength());
////    unsigned char responseBody[1024];
////    uint32_t responseBodyLen;
////    string ip = "127.0.0.1";
////    uint16_t port = 7881;
////    port = 7841;
////    socketSend(ip.c_str(),port,pdu.GetBuffer(),pdu.GetLength(),responseBody,responseBodyLen);
////    ASSERT_EQ(responseBodyLen> 0, true);
////    CImPdu *pPdu;
////    pPdu = CImPdu::ReadPdu(responseBody, responseBodyLen);
////    printf("len:%d\n",pPdu->GetLength());
////    printf("cid:%d\n",pPdu->GetCommandId());
////    ASSERT_EQ(pPdu->GetCommandId(), CID_FileImgDownloadRes);
////    PTP::File::FileImgDownloadRes msg_rsp;
////    msg_rsp.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());
////    printf("len:%lu\n",msg_rsp.file_data().size());
////    printf("data:\n\n%s\n",msg_rsp.file_data().c_str());
////
////}
//
//
//TEST(pt_net, socketSend) {
//    CSimpleBuffer request = *new CSimpleBuffer();
//    CSimpleBuffer response = *new CSimpleBuffer();
//    string ip = "127.0.0.1";
//    uint16_t port = 7841;
//    char const* pduBytes = "name";
//    request.Write((void *)pduBytes, strlen(pduBytes));
//    socketSend(ip.c_str(),port,&request,&response);
//    ASSERT_TRUE(true);
//}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
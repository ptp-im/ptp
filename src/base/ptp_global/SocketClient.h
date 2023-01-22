#ifndef FUNCTIONS_HTTP
#define FUNCTIONS_HTTP

#include "UtilPdu.h"
#include "UriHelper.h"
#include <string>
#include <algorithm>
#include <cstdint>
#include <iomanip>

#define SOCKET_SEND_BUF_SIZE        (2 * 1024 * 1024)

#define SOCKET_READ_BUF_SIZE		(2 * 1024 * 1024)

using namespace std;

enum SOCKET_CLIENT{
    SOCKET_CLIENT_OK = 0,
    SOCKET_CLIENT_INIT_ERROR,
    SOCKET_CLIENT_IP_CONVERT_ERROR,
    SOCKET_CLIENT_CONNECT_SERVER_ERROR,
};
int socketSend(const char *ip,uint16_t port,CSimpleBuffer *request,CSimpleBuffer *response);

//#define SERVICE_PORT_53 "53"
//
//struct HttpHeader
//{
//    string name;
//    string value;
//};
//
//enum HttpMethod{
//    HTTP_GET,
//    HTTP_POST
//};
//
//struct HttpRequest
//{
//    HttpMethod method;
//    bool isHttps;
//    string url;
//    string host;
//    string path;
//    uint16_t port;
//    string ip;
//    struct HttpHeader header[30];
//    unsigned char * responseBody;
//    uint32_t responseBodyLen;
//};
//
//struct HttpHeader httpHeader(const string &name, const string &value);

//int request(struct HttpRequest *httpRequest,unsigned char *requestBody,uint32_t requestBodyLen);
//int httpGet(const string& url,HttpRequest *httpHeader);
//int httpPost(const string& url,HttpRequest *HttpHeader,unsigned char *requestBody,uint32_t requestBodyLen);

//int resolveIpFromDomain(const string &domain, string &ip);
//void handleRequestUrl(const string &url,bool &isHttps,char *domain,uint16_t &port,char *path);

#endif
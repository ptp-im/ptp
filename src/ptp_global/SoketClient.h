#ifndef FUNCTIONS_HTTP
#define FUNCTIONS_HTTP

#include "UtilPdu.h"
#include <string>
#include <algorithm>
#include <cstdint>
#include <iomanip>

#define SOCKET_MAX_READ_BUF_SIZE        (2 * 1024 * 1024)
#define SOCKET_MAX_SEND_BUF_SIZE		(2 * 1024 * 1024)

#define SERVICE_PORT "53"

using namespace std;

struct Uri
{
public:
    std::string QueryString, Path, Protocol, Host;
    uint16_t Port;

    static Uri Parse(const std::string &uri)
    {
        Uri result;

        typedef std::string::const_iterator iterator_t;

        if (uri.length() == 0)
            return result;

        iterator_t uriEnd = uri.end();

        // get query start
        iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

        // protocol
        iterator_t protocolStart = uri.begin();
        iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');            //"://");

        if (protocolEnd != uriEnd)
        {
            std::string prot = &*(protocolEnd);
            if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
            {
                result.Protocol = std::string(protocolStart, protocolEnd);
                protocolEnd += 3;   //      ://
            }
            else
                protocolEnd = uri.begin();  // no protocol
        }
        else
            protocolEnd = uri.begin();  // no protocol

        // host
        iterator_t hostStart = protocolEnd;
        iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

        iterator_t hostEnd = std::find(protocolEnd,
                                       (pathStart != uriEnd) ? pathStart : queryStart,
                                       ':');  // check for port

        result.Host = std::string(hostStart, hostEnd);

        // port
        if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
        {
            hostEnd++;
            iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
            result.Port = (uint16_t)stoi(std::string(hostEnd, portEnd));
        }

        // path
        if (pathStart != uriEnd)
            result.Path = std::string(pathStart, queryStart);

        // query
        if (queryStart != uriEnd)
            result.QueryString = std::string(queryStart, uri.end());

        return result;

    }   // Parse
};  // uri

struct HttpHeader
{
    string name;
    string value;
};

enum HttpMethod{
    HTTP_GET,
    HTTP_POST
};

struct HttpRequest
{
    HttpMethod method;
    bool isHttps;
    string url;
    string host;
    string path;
    uint16_t port;
    string ip;
    struct HttpHeader header[30];
    unsigned char * responseBody;
    uint32_t responseBodyLen;
};

struct HttpHeader httpHeader(const string &name, const string &value);

int request(struct HttpRequest *httpRequest,unsigned char *requestBody,uint32_t requestBodyLen);
int socketSend(const char *ip,uint16_t port,CSimpleBuffer *request,CSimpleBuffer *response);
int httpGet(const string& url,HttpRequest *httpHeader);
int httpPost(const string& url,HttpRequest *HttpHeader,unsigned char *requestBody,uint32_t requestBodyLen);

int resolveIpFromDomain(const string &domain, string &ip);
void handleRequestUrl(const string &url,bool &isHttps,char *domain,uint16_t &port,char *path);

#endif
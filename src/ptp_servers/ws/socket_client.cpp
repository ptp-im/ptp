#include <cstdio>           // sprintf
#include <cstring>          // memset strncpy
#include <sys/socket.h>      // socket connect
#include <netinet/in.h>      // all socket funtions
#include <arpa/inet.h>       // inet_pton htons
#include <unistd.h>          // write
#include <netdb.h>           // hostent addrinfo
#include "socket_client.h"
#include "UtilPdu.h"

#define SA struct sockaddr

int resolveIpFromDomain(const string &domain, string &ip)
{
    while (true){
        int err, sock;
        char data_sent[15];
        struct addrinfo hints, *result;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;      // Use only IPV4 to avoid network unreachable error
        hints.ai_socktype = SOCK_DGRAM; // Use UDP PROTOCOL to connect to the DNS on port 53
        err = getaddrinfo(domain.data(), SERVICE_PORT, &hints, &result);
        if (err != 0){
            printf("An error has occurred while getting address. %s.", gai_strerror(err));
            break;
        }
        sock = socket(result->ai_family, result->ai_socktype, 0);
        if (sock < 0){
            printf("A socket error has occurred");
            break;
        }
        err = (int)sendto(sock, data_sent, sizeof(data_sent), 0, result->ai_addr, result->ai_addrlen);
        if (err < 0){
            printf("failed to send request to server");
            break;
        }
        struct sockaddr_in *addr;
        addr = (struct sockaddr_in *)result->ai_addr;
        char ip1[addr->sin_len];
        strcpy(ip1, inet_ntoa((struct in_addr)addr->sin_addr));
        ip.append(ip1);

        close(sock);
        freeaddrinfo(result);
        return 0;
    }

    return 1;
}

int httpGet(const string& url, struct HttpRequest *httpRequest){
    httpRequest->url.assign(url);
    httpRequest->method = HTTP_GET;
    return request(httpRequest, nullptr,0);
}

int httpPost(const string& url,struct HttpRequest *httpRequest,unsigned char *requestBody,uint32_t requestBodyLen){
    httpRequest->url.assign(url);
    httpRequest->method = HTTP_POST;
    return request(httpRequest, requestBody, requestBodyLen);
}

int socketSend(const char* ip,uint16_t port,unsigned char *requestBody,uint32_t requestBodyLen,unsigned char *responseBody,uint32_t &responseBodyLen){

    while (true){
        int sock;
        struct sockaddr_in server{};

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) // Init TCP socket
        {
            printf("Failed to create Socket");
            break;
        }
        memset(&server, 0, sizeof(server));

        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if ((inet_pton(AF_INET, ip, &server.sin_addr)) <= 0){
            printf("Unable to convert ip address to binary form");
            break;
        }

        if ((connect(sock, (SA *)&server, sizeof(server))) < 0){
            printf("Failed to connect to the server");
            break;
        }

        unsigned char *requestBuf = new unsigned char[requestBodyLen + 1];
        memcpy(requestBuf,requestBody,requestBodyLen);

        uint32_t offset = 0;
        uint32_t remain = requestBodyLen;
        uint32_t err = 0;
        while (remain > 0) {
            uint32_t send_size = remain;
            if (send_size > SOCKET_MAX_SEND_BUF_SIZE) {
                send_size = SOCKET_MAX_SEND_BUF_SIZE;
            }
            auto ret = write(sock, requestBuf+offset, send_size);
            if (ret == SOCKET_ERROR){
                err = 1;
                break;
            }
            //printf("Send len: %lu,remain: %d,send_size: %d,ret: %d,offset: %d,total: %d\n",requestBodyLen,remain,send_size,ret,offset,requestBodyLen-remain);
            if (ret <= 0) {
                ret = 0;
                break;
            }
            offset += ret;
            remain -= ret;
        }

        if(err){
            break;
        }

        CSimpleBuffer m_in_buf = *new CSimpleBuffer();
        for (;;)
        {
            uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
            if (free_buf_len < SOCKET_MAX_READ_BUF_SIZE + 1)
                m_in_buf.Extend(SOCKET_MAX_READ_BUF_SIZE + 1);
            int ret = (int)recv(sock, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(),SOCKET_MAX_READ_BUF_SIZE,0);
            printf("http request ret=%d,offset=%d\n",ret,m_in_buf.GetWriteOffset());
            if (ret <= 0)
                break;
            m_in_buf.IncWriteOffset(ret);
        }
        responseBodyLen =  m_in_buf.GetWriteOffset();
        printf("responseBodyLen:%d\n",responseBodyLen);
        unsigned char *in_buf = m_in_buf.GetBuffer();
        memcpy(responseBody,in_buf,responseBodyLen);
        return 0;
    }

    return 1;
}

int request(struct HttpRequest *httpRequest,unsigned char *requestBody,uint32_t requestBodyLen)
{
    while (true){
        int sock;
        struct sockaddr_in server{};
        Uri uri = Uri::Parse(httpRequest->url);
        httpRequest->isHttps = uri.Protocol == "https";
        httpRequest->host = uri.Host;
        httpRequest->path = uri.Path.empty() ? "/" : uri.Path ;
        httpRequest->port = uri.Port;

        if (resolveIpFromDomain(httpRequest->host, httpRequest->ip)){
            printf("An error has occurred fetching the ip of the domain");
            break;
        }

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) // Init TCP socket
        {
            printf("Failed to create Socket");
            break;
        }
        memset(&server, 0, sizeof(server));

        server.sin_family = AF_INET;
        server.sin_port = htons(httpRequest->port);

        if ((inet_pton(AF_INET, httpRequest->ip.data(), &server.sin_addr)) <= 0){
            printf("Unable to convert ip address to binary form");
            break;
        }

        if ((connect(sock, (SA *)&server, sizeof(server))) < 0){
            printf("Failed to connect to the server");
            break;
        }
        string contentType = "multipart/form-data";


        string method;
        switch (httpRequest->method) {
            case HTTP_POST:
                method = "POST";
                break;
            case HTTP_GET:
            default:
                method = "GET";
                break;
        }
        const char * headerLineGapBuff = "\r\n";

        char *headerBuf = new char[1024];
        strcat(headerBuf,method.data());
        strcat(headerBuf," ");
        strcat(headerBuf,httpRequest->path.data());
        strcat(headerBuf," ");
        strcat(headerBuf,"HTTP/1.1");
        strcat(headerBuf,headerLineGapBuff);
        strcat(headerBuf,"Host: ");
        strcat(headerBuf,httpRequest->host.data());
        strcat(headerBuf,headerLineGapBuff);
        strcat(headerBuf,"Content-Type: ");
        strcat(headerBuf,contentType.data());
        strcat(headerBuf,headerLineGapBuff);

        if(requestBodyLen > 0){
            strcat(headerBuf,"Content-Length: ");
            strcat(headerBuf,to_string(requestBodyLen).c_str());
            strcat(headerBuf,headerLineGapBuff);
            strcat(headerBuf,headerLineGapBuff);
        }
        //printf("%s\n",headerBuf);
        uint32_t headerLen = strlen(headerBuf);
        //printf("requestBodyLen:%d\n",requestBodyLen);
        uint32_t requestLen = requestBodyLen + headerLen;
        //printf("headerLen:%d\n",headerLen);
        //printf("requestLen:%d\n",requestLen);
        char *requestBuf = new char[requestLen + 1];
        strcpy(requestBuf,headerBuf);
        memcpy(requestBuf+headerLen,(char *)requestBody,requestBodyLen);

        uint32_t offset = 0;
        uint32_t remain = requestLen;
        uint32_t err = 0;
        while (remain > 0) {
            uint32_t send_size = remain;
            if (send_size > SOCKET_MAX_SEND_BUF_SIZE) {
                send_size = SOCKET_MAX_SEND_BUF_SIZE;
            }

            auto ret = write(sock, requestBuf+offset, send_size);
            if (ret == SOCKET_ERROR){
                err = 1;
                break;
            }
            //printf("Send len: %lu,remain: %d,send_size: %d,ret: %d,offset: %d,total: %d\n",requestLen,remain,send_size,ret,offset,requestLen-remain);
            if (ret <= 0) {
                ret = 0;
                break;
            }
            offset += ret;
            remain -= ret;
        }

        if(err){
            break;
        }

        CSimpleBuffer m_in_buf = *new CSimpleBuffer();
        for (;;)
        {
            uint32_t free_buf_len = m_in_buf.GetAllocSize() - m_in_buf.GetWriteOffset();
            if (free_buf_len < SOCKET_MAX_READ_BUF_SIZE + 1)
                m_in_buf.Extend(SOCKET_MAX_READ_BUF_SIZE + 1);
            int ret = (int)recv(sock, m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(),SOCKET_MAX_READ_BUF_SIZE,0);
            //printf("http request ret=%d,offset=%d\n",ret,m_in_buf.GetWriteOffset());
            if (ret <= 0)
                break;
            m_in_buf.IncWriteOffset(ret);
        }
        //printf("%s\n",m_in_buf.GetBuffer());

        httpRequest->responseBodyLen =  m_in_buf.GetWriteOffset();
        printf("responseBodyLen:%d\n",httpRequest->responseBodyLen);
        unsigned char *in_buf = m_in_buf.GetBuffer();
        memcpy(httpRequest->responseBody,in_buf,httpRequest->responseBodyLen);

        return 0;
    }

    return 1;
}

struct HttpHeader httpHeader(const string &name, const string &value)
{
    struct HttpHeader header;
    header.name = name;
    header.value = value;
    return header;
}

//
//#include "stdio.h" // printf(), exit()
//
//#include "openssl/bio.h"
//#include "openssl/err.h"
//#include "openssl/ssl.h"
//
////sample ssl https connection
//int tls_connect(char *hostname)
//{
//    /* SSL context structure */
//    SSL_CTX *ctx;
//    /* SSL connection structure */
//    SSL *ssl;
//    /* connection BIO */
//    BIO *cbio;
//
//    /* addressee of the connection */
//    char name[1024];
//    /* request */
//    char req[1024];
//    /* response */
//    char resp[1024];
//    int len;
//
//    /* Set up the OpenSSL library */
//
//    SSL_load_error_strings();
//    SSL_library_init();
//
//    //OpenSSL_add_all_algorithms();
//
//    /* Create SSL context structure and load the trust store
//     * (accepted root ca-certificates) */
//
//    ctx = SSL_CTX_new(SSLv23_client_method());
//    if (!ctx)
//    {
//        fprintf(stderr, "Error creating SSL context\n");
//        ERR_print_errors_fp(stderr);
//        exit(1);
//    }
//
//    /* Set up the SSL connection */
//
//    cbio = BIO_new_ssl_connect(ctx);
//
//    /* Set flag SSL_MODE_AUTO_RETRY */
//    BIO_get_ssl(cbio, &ssl);
//    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
//
//    /* Connect to server <hostname> */
//
//    /* name = "<hostname>:<port>" */
//    sprintf(name, "%s:%s", hostname, "https");
//
//    BIO_set_conn_hostname(cbio, name);
//
//    if (BIO_do_connect(cbio) <= 0)
//    {
//        fprintf(stderr, "Error attempting to connect\n");
//        ERR_print_errors_fp(stderr);
//        BIO_free_all(cbio);
//        SSL_CTX_free(ctx);
//        exit(1);
//    }
//
//    /* Check the certificate */
//
//    if (SSL_get_verify_result(ssl) != X509_V_OK)
//    {
//        fprintf(stderr,
//                "Certificate verification error: %i\n",
//                (int)SSL_get_verify_result(ssl));
//        /* continue */
//    }
//
//    /* send HTTP request to the server <hostname> */
//
//    sprintf(req,
//            "GET / HTTP/1.0\x0D\x0AHost: %s\x0D\x0A\x43onnection: Close\x0D\x0A\x0D\x0A",
//            hostname);
//    BIO_puts(cbio, req);
//
//    /* read HTTP response from server and print to stdout */
//
//    for (;;)
//    {
//        len = BIO_read(cbio, resp, 1024);
//        if (len <= 0)
//        {
//            break;
//        }
//        resp[len] = '\0';
//        printf("%s", resp);
//    }
//    printf("\n");
//    /* close TCP/IP connection and free bio and context */
//
//    BIO_free_all(cbio);
//    SSL_CTX_free(ctx);
//
//    return 0;
//}
//
//int main(int argc, char *argv[])
//{
//    if (argc > 1)
//    {
//        tls_connect(argv[1]);
//    }
//    else
//    {
//        printf("Usage: %s <hostname>\n", argv[0]);
//    }
//    return 0;
//}
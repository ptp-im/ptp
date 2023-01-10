#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>
#include <curl/curl.h>
#include "ptp_global/global.h"
#include <iostream>
using namespace std;

class CHttpClient
{
public:
    CHttpClient(void);
    ~CHttpClient(void);
    
public:
    CURLcode Post(const string & strUrl, const string & strPost, string & strResponse);
    CURLcode Get(const string & strUrl, string & strResponse);
    string UploadByteFile(const string &strUrl, string& content,const string &fileName,const string &fileType);
    string UploadByteFile(const string &url, void* data, int data_len);
    //bool DownloadByteFile(const string &url, AudioMsgInfo* pAudioMsg);
    bool DownloadByteFile(const string &url, char* pAudioMsg);
};

#endif
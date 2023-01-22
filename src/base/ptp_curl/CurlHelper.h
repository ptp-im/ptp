//
// Created by jack on 2023/1/22.
//

#ifndef PTP_CTL_CURLHELPER_H
#define PTP_CTL_CURLHELPER_H


#include <cstdio>
#include <string>
#include "curl/curl.h"

using namespace std;

size_t curl_write_data_string(void *ptr, size_t size, size_t nMemBuf, void *rsp);

size_t curl_write_data_binary(void *ptr, size_t size, size_t nMemBuf, void* outBuf);

uint16_t curl_get(const string &url,string &response);

uint16_t curl_post(const string &url,void *postData,uint32_t size,string &response);

#endif //PTP_CTL_CURLHELPER_H

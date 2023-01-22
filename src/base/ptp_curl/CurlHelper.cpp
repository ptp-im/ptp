//
// Created by jack on 2023/1/22.
//

#include "CurlHelper.h"
#include "ptp_global/Logger.h"


size_t curl_write_data_string(void *ptr, size_t size, size_t nMemBuf, void *rsp)
{
    size_t len = size * nMemBuf;
    auto* response = (string *)rsp;
    response->append((char*)ptr, len);
    return len;
}

size_t curl_write_data_binary(void *ptr, size_t size, size_t nMemBuf, void* outBuf)
{
    size_t nLen = size * nMemBuf;
    memcpy(outBuf, ptr, nLen);
    return nLen;
}

uint16_t curl_get(const string &url,string &response){
    while (true){
        CURL* curl = curl_easy_init();
        struct curl_slist *headerList = nullptr;
        headerList = curl_slist_append(headerList, "user-agent: Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);    //enable verbose for easier tracing
        string strResp;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (CURLE_OK != res) {
            DEBUG_E("curl_easy_perform failed, res=%d\n", res);
            return res;
        }
        return CURLE_OK;
    }
}

uint16_t curl_post(const string &url,void *postData,uint32_t size,void *response){
    while (true){
        CURL* curl = curl_easy_init();
        struct curl_slist *headerList = nullptr;
        headerList = curl_slist_append(headerList, "Content-Type: multipart/form-data");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);    //enable verbose for easier tracing
        string body;
        body.append((char*)postData, size);             // image buffer
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
        string strResp;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (CURLE_OK != res) {
            DEBUG_E("curl_easy_perform failed, res=%d\n", res);
            return res;
        }
        return CURLE_OK;
    }
}


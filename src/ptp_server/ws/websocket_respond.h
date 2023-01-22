#ifndef __WEBSOCKET_RESPOND__
#define __WEBSOCKET_RESPOND__

#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include "ptp_global/Utils.h"
#include "ptp_crypto/sha1.h"
#include "ptp_crypto/Base64Utils.h"

#define MAGIC_KEY "258EAwA5-E914-473A-95CA-C5AB3DC85B11"

typedef std::map<std::string, std::string> HEADER_MAP;

class Websocket_Respond {
public:
    HEADER_MAP header_map_;
	Websocket_Respond();
	~Websocket_Respond();
	uint64_t ntohll1(uint64_t val);
	uint64_t htonll1(uint64_t val);

    int fetch_http_info(char* buff_);
	char* toFrameDataPkt(const std::string &inMessage);
    void parse_str(char *request);
};

#endif

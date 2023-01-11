#ifndef __WEBSOCKET_RESPOND__
#define __WEBSOCKET_RESPOND__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include "ptp_global/Utils.h"

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef std::map<std::string, std::string> HEADER_MAP;

class WebsocketRespond {
public:
    HEADER_MAP header_map_;
	WebsocketRespond();
	~WebsocketRespond();
	uint64_t ntohll1(uint64_t val);
	uint64_t htonll1(uint64_t val);

    int fetch_http_info(char* buff_);
	char* toFrameDataPkt(const std::string &inMessage);
    void parse_str(char *request);
};

#endif

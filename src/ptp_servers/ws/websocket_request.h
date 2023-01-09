#ifndef __WEBSOCKET_REQUEST__
#define __WEBSOCKET_REQUEST__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "ptp_global/Util.h"

#define  MAX_WEBSOCKET_BUFFER 30 * 1024 * 1024

// 数据包操作类型
enum WebSocketOpCode {
    ContinuationFrame = 0x0,			//连续帧
    TextFrame = 0x1,					//文本帧
    BinaryFrame = 0x2,					//二进制帧
    ConnectionClose = 0x8,				//连接关闭
    Ping = 0x9,
    Pong = 0xA
};

//Websocket数据包数据头信息
struct WebSocketStreamHeader {
    unsigned int header_size;				//数据包头大小
    int mask_offset;					    //掩码偏移
    unsigned int payload_size;				//数据大小
    bool fin;                               //帧标记
    bool masked;					        //掩码
    unsigned char opcode;					//操作码
    unsigned char res[3];
};

class Websocket_Request {
public:
	Websocket_Request();
	~Websocket_Request();
	int fetch_websocket_info(char *msg);
	void print();
	void reset();

private:
    int fetch_fin(char *msg, int &pos);
	int fetch_opcode(char *msg, int &pos);
	int fetch_mask(char *msg, int &pos);
	int fetch_masking_key(char *msg, int &pos);
	int fetch_payload_length(char *msg, int &pos);
	int fetch_payload(char *msg, int &pos);
public:
	uint8_t fin_;
	uint8_t opcode_;
	uint8_t mask_;
	uint8_t masking_key_[4];
	uint64_t payload_length_;
	char payload_[20480];
	char *alldata;
};

#endif

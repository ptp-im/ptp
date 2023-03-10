#include "WebsocketRequest.h"
#include "ptp_global/Logger.h"


WebsocketRequest::WebsocketRequest():
		fin_(),
		opcode_(),
		mask_(),
		masking_key_(),
		payload_length_(),
		payload_()
{
}

WebsocketRequest::~WebsocketRequest(){
	reset();
}

int WebsocketRequest::fetch_websocket_info(char *msg){
	alldata=msg;
	int pos = 0;
	fetch_fin(msg, pos);
	fetch_opcode(msg, pos);
	fetch_mask(msg, pos);
	fetch_payload_length(msg, pos);
	fetch_masking_key(msg, pos);
	return fetch_payload(msg, pos);
}

void WebsocketRequest::print(){
	printf("WEBSOCKET PROTOCOL\n"
				"FIN: %d\n"
				"OPCODE: %d\n"
				"MASK: %d\n"
				"PAYLOADLEN: %ld\n"
				"PAYLOAD: %s",
				fin_, opcode_, mask_, (long )payload_length_, payload_);

    printf("WEBSOCKET BODY %s",payload_);
}

void WebsocketRequest::reset(){
	fin_ = 0;
	opcode_ = 0;
	mask_ = 0;
	memset(masking_key_, 0, sizeof(masking_key_));
	payload_length_ = 0;
	memset(payload_, 0, sizeof(payload_));
}

int WebsocketRequest::fetch_fin(char *msg, int &pos){
	fin_ = (unsigned char)msg[0] >> 7;
	return 0;
}

int WebsocketRequest::fetch_opcode(char *msg, int &pos){
	opcode_ = msg[0] & 0x0f;
	pos++;
	return 0;
}

int WebsocketRequest::fetch_mask(char *msg, int &pos){
	mask_ = (unsigned char)msg[1] >> 7;
	return 0;
}

int WebsocketRequest::fetch_masking_key(char *msg, int &pos){
	if(mask_ != 1)
	{
		return 0;
	}

	for(int i = 0; i < 4; i++)
		masking_key_[i] = msg[pos + i];
	pos += 4;
	return 0;
}

int WebsocketRequest::fetch_payload_length(char *msg, int &pos){
	payload_length_ = msg[pos] & 0x7f;
	pos++;
	if(payload_length_ == 126){
		uint16_t length = 0;
		memcpy(&length, msg + pos, 2);
		pos += 2;
		payload_length_ = ntohs(length);
        DEBUG_D("126 payload_length_: %d", payload_length_);
    }
	else if(payload_length_ == 127){
//		uint32_t length = 0;
//		memcpy(&length, msg + pos, 4);
//		pos += 4;
//		payload_length_ = ntohl(length);
        uint64_t length = 0;
        memcpy(&length, msg + pos, 8);
        pos += 8;
        payload_length_ = (size_t)ntohll(length);
        DEBUG_D("127 payload_length_: %d", payload_length_);
    }
	return 0;
}

int WebsocketRequest::fetch_payload(char *msg, int &pos){
	memset(payload_, 0, sizeof(payload_));
    if(mask_ != 1){
		memcpy(payload_, msg + pos, payload_length_);
	}
	else {
		for(uint i = 0; i < payload_length_; i++){
			int j = i % 4;
			payload_[i] = msg[pos + i] ^ masking_key_[j];
		}
	}
//	//????????????????????????????????????????????????????????????\0???
//	if(opcode_==0x1)
//	{
//		payload_[payload_length_]='\0';
//	}
	pos += payload_length_;

    return 0;
}



bool ReadHeader(const unsigned char* cData, WebSocketStreamHeader* header)
{
    if (cData == NULL)return false;

    const unsigned char *buf = cData;

    header->fin = buf[0] & 0x80;
    header->masked = buf[1] & 0x80;
    unsigned char stream_size = buf[1] & 0x7F;

    header->opcode = buf[0] & 0x0F;
    if (header->opcode == WebSocketOpCode::ContinuationFrame) {
        //?????????
        return false;
    }
    else if (header->opcode == WebSocketOpCode::TextFrame) {
        //?????????
    }
    else if (header->opcode == WebSocketOpCode::BinaryFrame) {
        //????????????

    }
    else if (header->opcode == WebSocketOpCode::ConnectionClose) {
        //??????????????????
        return false;
    }
    else if (header->opcode == WebSocketOpCode::Ping) {
        //  ping
        return false;
    }
    else if (header->opcode == WebSocketOpCode::Pong) {
        // pong
        return false;
    }
    else {
        //?????????
        return false;
    }

    if (stream_size <= 125) {
        //	small stream
        header->header_size =6;
        header->payload_size = stream_size;
        header->mask_offset = 2;
    }
    else if (stream_size == 126) {
        //	medium stream
        header->header_size = 8;
        unsigned short s = 0;
        memcpy(&s, (const char*)&buf[2], 2);
        header->payload_size = ntohs(s);
        header->mask_offset = 4;
    }
    else if (stream_size == 127) {

        unsigned long long l = 0;
        memcpy(&l, (const char*)&buf[2], 8);

        header->payload_size = l;
        header->mask_offset = 10;
    }
    else {
        //Couldnt decode stream size ?????????????????????
        return false;
    }

    if (header->payload_size > MAX_WEBSOCKET_BUFFER) {
        return false;
    }

    return true;
}

#ifndef HTTPPDU_H_
#define HTTPPDU_H_

#include "ptp_global/Utils.h"
#include "ptp_protobuf/ImPdu.h"
#include <list>
#include "IM.BaseDefine.pb.h"

// jsonp parameter parser
class CPostDataParser
{
public:
	CPostDataParser() {}
	virtual ~CPostDataParser() {}

	bool Parse(const char* content);

	char* GetValue(const char* key);
private:
	std::map<std::string, std::string> m_post_map;
};

char* PackSendResult(uint32_t error_code, const char* error_msg = "");
char* PackSendCreateGroupResult(uint32_t error_code, const char* error_msg, uint32_t group_id);
char* PackGetUserIdByNickNameResult(uint32_t result, std::list<IM::BaseDefine::UserInfo> user_list);
#endif /* HTTPPDU_H_ */

#ifndef CACHEPOOL_H_
#define CACHEPOOL_H_

#include <vector>
#include "ptp_global/Utils.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/ThreadPool.h"
#include "ptp_global/Logger.h"
#include "hiredis.h"

#define     MEMBER_GROUP_UNREAD_KEY_PREFIX              "member_group_unread:"
#define     MEMBER_GROUP_UPDATE_KEY_PREFIX              "member_group_update:"
#define     MEMBER_PAIR_UPDATE_PREFIX                   "member_pare_update:"
#define     GROUP_MEMBER_STATUS_KEY_PREFIX              "group_member_status:"
#define     GROUP_MEMBER_UPDATE_KEY_PREFIX              "group_member_update:"
#define     GROUP_ID_INCR_PREFIX                        "group_id_incr"
#define     GROUP_MSG_ID_BY_ID_INCR_PREFIX              "group_msg_id_by_id_incr:"
#define     GROUP_MSG_DATA_BY_ID_PREFIX                 "group_msg_data_by_id:"
#define     GROUP_LATEST_MSG_DATA_BY_ID_PREFIX          "group_latest_msg_data_by_id:"
#define     GROUP_IDX_BY_UID_INCR_PREFIX                "group_idx_by_uid_incr:"
#define     GROUP_PAIR_USER_REL_PREFIX                  "group_pair_user_rel:"
#define     GROUP_ADDRESS_ID_PREFIX                     "group_adr_id:"
#define     GROUP_ADDRESS_PREFIX                        "group_adr:"
#define     GROUP_UPDATE_PREFIX                         "group_update:"
#define     GROUP_CREATE_PREFIX                         "group_create:"
#define     GROUP_ABOUT_PREFIX                          "group_about:"
#define     GROUP_NAME_PREFIX                           "group_name:"
#define     GROUP_AVATAR_PREFIX                         "group_avatar:"
#define     GROUP_OWNER_PREFIX                          "group_owner:"
#define     GROUP_PAIR_PREFIX                           "group_pair:"
#define     GROUP_MSG_UPDATE_PREFIX                     "group_msg_update:"
#define     GROUP_TYPE_PREFIX                           "group_type:"
#define     GROUP_IDX_PREFIX                            "group_idx:"
#define     GROUP_INFO_PREFIX                           "group_info:"
#define     MEMBER_GROUP_KEY_PREFIX                     "member_group:"

#define     CACHE_USER_ADR_ID_PREFIX                    "user_adr_id_prefix:"
#define     CACHE_USER_USERNAME_UID_PREFIX              "user_username_uid_prefix:"
#define     CACHE_INCR_USER_ID                          "user_id_incr:"
#define     CACHE_USER_ID_PUB_KEY_PREFIX                "user_id_pub_key_prefix:"
#define     CACHE_USER_ID_ADR_PREFIX                    "user_id_adr_prefix:"

#define     CACHE_USER_ID_AVATAR_PREFIX                 "user_id_avatar_prefix:"
#define     CACHE_USER_ID_NICKNAME_PREFIX               "user_id_nickname_prefix:"
#define     CACHE_USER_ID_USERNAME_PREFIX               "user_id_username_prefix:"
#define     CACHE_USER_ID_SIGN_INFO_PREFIX              "user_id_sign_info_prefix:"
#define     CACHE_USER_ID_FIRST_NAME_PREFIX             "user_id_first_name_prefix:"
#define     CACHE_USER_ID_LAST_NAME_PREFIX              "user_id_last_name_prefix:"
#define     CACHE_USER_ID_STATUS_PREFIX                 "user_id_status_prefix:"
#define     CACHE_USER_ID_LOGIN_TIME_PREFIX             "user_id_login_time_prefix:"

#define     CACHE_GROUP_INSTANCE                        "group"
#define     CACHE_AUTH_INSTANCE                         "auth"

class CachePool;

class CacheConn {
public:
	CacheConn(CachePool* pCachePool);
	virtual ~CacheConn();

	int Init();
	const char* GetPoolName();

	string get(string key);
	string setex(string key, int timeout, string value);
    string set(string key, string& value);

    //批量获取
    bool mget(const vector<string>& keys, map<string, string>& ret_value);
    // 判断一个key是否存在
    bool isExists(string &key);

	// Redis hash structure
	long hdel(string key, string field);
	string hget(string key, string field);
	bool hgetAll(string key, map<string, string>& ret_value);
	long hset(string key, string field, string value);

	long hincrBy(string key, string field, long value);
    long incrBy(string key, long value);
	string hmset(string key, map<string, string>& hash);
	bool hmget(string key, list<string>& fields, list<string>& ret_value);

    bool cmd(string cmd_name,string key, list<string>* argv_list, list<string>* ret_list );
    bool exec(list<string>* argv_list,list<string>* ret_list );

    //原子加减1
    long incr(string key);
    long decr(string key);

	// Redis list structure
	long lpush(string key, string value);
	long rpush(string key, string value);
	long llen(string key);
	bool lrange(string key, long start, long end, list<string>& ret_value);

private:
	CachePool* 		m_pCachePool;
	redisContext* 	m_pContext;
	uint64_t		m_last_connect_time;
};

class CachePool {
public:
    CachePool(const char *pool_name, const char *server_ip, int server_port,const char *auth, int db_num, int max_conn_cnt);

    virtual ~CachePool();

	int Init();

	CacheConn* GetCacheConn();
	void RelCacheConn(CacheConn* pCacheConn);

    const char *GetPoolName() { return m_pool_name.c_str(); }
    const char *GetAuth() { return m_auth.c_str(); }
    const char *GetServerIP() { return m_server_ip.c_str(); }

    int GetServerPort() { return m_server_port; }

    int GetDBNum() { return m_db_num; }

private:
    string m_pool_name;
    string m_server_ip;
    string m_auth;
    int m_server_port;
    int m_db_num;

	int			m_cur_conn_cnt;
	int 		m_max_conn_cnt;
	list<CacheConn*>	m_free_list;
	CThreadNotify		m_free_notify;
};

class CacheManager {
public:
	virtual ~CacheManager();

	static CacheManager* getInstance();
    static void setConfigPath(const string &path);

	int Init();
	CacheConn* GetCacheConn(const char* pool_name);
	void RelCacheConn(CacheConn* pCacheConn);
private:
	CacheManager();

private:
	static CacheManager* 	s_cache_manager;
	map<string, CachePool*>	m_cache_pool_map;
};

#endif /* CACHEPOOL_H_ */

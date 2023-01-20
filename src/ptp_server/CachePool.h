#ifndef CACHEPOOL_H_
#define CACHEPOOL_H_

#include <vector>
#include "ptp_global/Utils.h"
#include "ptp_global/Helpers.h"
#include "ptp_global/ThreadPool.h"
#include "ptp_global/Logger.h"
#include "hiredis.h"

#define     MEMBER_GROUP_UNREAD_KEY_PREFIX              "MGUN:"
#define     MEMBER_GROUP_UPDATE_KEY_PREFIX              "MGUP:"
#define     MEMBER_PAIR_UPDATE_PREFIX                   "MPUP:"
#define     GROUP_MEMBER_STATUS_KEY_PREFIX              "GMST:"
#define     GROUP_MEMBER_UPDATE_KEY_PREFIX              "GMUP:"
#define     GROUP_ID_INCR_PREFIX                        "GRII"
#define     GROUP_MSG_ID_BY_ID_INCR_PREFIX              "GMII:"
#define     GROUP_MSG_DATA_BY_ID_PREFIX                 "GMDI:"
#define     GROUP_LATEST_MSG_DATA_BY_ID_PREFIX          "GLMI:"
#define     GROUP_IDX_BY_UID_INCR_PREFIX                "GIUI:"
#define     GROUP_PAIR_USER_REL_PREFIX                  "GPUR:"
#define     GROUP_ADDRESS_ID_PREFIX                     "GRAI:"
#define     GROUP_ADDRESS_PREFIX                        "GRAD:"
#define     GROUP_UPDATE_PREFIX                         "GRUP:"
#define     GROUP_CREATE_PREFIX                         "GRCR:"
#define     GROUP_ABOUT_PREFIX                          "GRAB:"
#define     GROUP_NAME_PREFIX                           "GRNA:"
#define     GROUP_AVATAR_PREFIX                         "GRAV:"
#define     GROUP_OWNER_PREFIX                          "GROW:"
#define     GROUP_PAIR_PREFIX                           "GRPA:"
#define     GROUP_MSG_UPDATE_PREFIX                     "GRMU:"
#define     GROUP_TYPE_PREFIX                           "GRTY:"
#define     GROUP_IDX_PREFIX                            "GIDX:"
#define     GROUP_INFO_PREFIX                           "GRIN:"
#define     MEMBER_GROUP_KEY_PREFIX                     "MGRO:"

#define     CACHE_USER_ADR_ID_PREFIX                    "UAID:"
#define     CACHE_USER_USERNAME_UID_PREFIX              "UUUI:"
#define     CACHE_INCR_USER_ID                          "IUID"
#define     CACHE_USER_ID_PUB_KEY_PREFIX                "UIPU:"
#define     CACHE_USER_ID_ADR_PREFIX                    "UIAD:"

#define     CACHE_USER_ID_AVATAR_PREFIX                 "UIAV:"
#define     CACHE_USER_ID_NICKNAME_PREFIX               "UINI:"
#define     CACHE_USER_ID_USERNAME_PREFIX               "UIUS:"
#define     CACHE_USER_ID_SIGN_INFO_PREFIX              "UISI:"
#define     CACHE_USER_ID_FIRST_NAME_PREFIX             "UIFN:"
#define     CACHE_USER_ID_LAST_NAME_PREFIX              "UILN:"
#define     CACHE_USER_ID_STATUS_PREFIX                 "UIST:"
#define     CACHE_USER_ID_LOGIN_TIME_PREFIX             "UILT:"

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

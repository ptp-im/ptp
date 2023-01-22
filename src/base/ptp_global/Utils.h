#ifndef __UTIL_H__
#define __UTIL_H__
#include "global.h"
#include "UtilPdu.h"
#include "Lock.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifndef _WIN32
    #include <strings.h>
#endif

#include <sys/stat.h>
#include <cassert>

#ifdef _WIN32
    #define	snprintf	sprintf_s
#else
    #include <cstdarg>
    #include <pthread.h>
    #include <ctime>
    #include <sys/time.h>
#endif

#define NOTUSED_ARG(v) ((void)v)		// used this to remove warning C4100, unreferenced parameter
#define _CRT_SECURE_NO_DEPRECATE	// remove warning C4996,

class CRefObject
{
public:
	CRefObject();
	virtual ~CRefObject();

	void SetLock(CLock* lock) { m_lock = lock; }
	void AddRef();
	void ReleaseRef();
private:
	int				m_refCount;
	CLock*	m_lock;
};


class CStrExplode
{
public:
	CStrExplode(char* str, char seperator);
	virtual ~CStrExplode();

	uint32_t GetItemCnt() { return m_item_cnt; }
	char* GetItem(uint32_t idx) { return m_item_list[idx]; }
private:
	uint32_t	m_item_cnt;
	char** 		m_item_list;
};

#endif

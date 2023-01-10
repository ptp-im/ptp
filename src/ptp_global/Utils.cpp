#include "Utils.h"
#include <sstream>
#include <fcntl.h>

using namespace std;

CRefObject::CRefObject()
{
	m_lock = NULL;
	m_refCount = 1;
}

CRefObject::~CRefObject()
{

}

void CRefObject::AddRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount++;
		m_lock->unlock();
	}
	else
	{
		m_refCount++;
	}
}

void CRefObject::ReleaseRef()
{
	if (m_lock)
	{
		m_lock->lock();
		m_refCount--;
		if (m_refCount == 0)
		{
			delete this;
			return;
		}
		m_lock->unlock();
	}
	else
	{
		m_refCount--;
		if (m_refCount == 0){
            delete this;
        }

	}
}


CStrExplode::CStrExplode(char* str, char seperator)
{
    m_item_cnt = 1;
    char* pos = str;
    while (*pos) {
        if (*pos == seperator) {
            m_item_cnt++;
        }

        pos++;
    }

    m_item_list = new char* [m_item_cnt];

    int idx = 0;
    char* start = pos = str;
    while (*pos) {
        if ( pos != start && *pos == seperator) {
            uint32_t len = pos - start;
            m_item_list[idx] = new char [len + 1];
            strncpy(m_item_list[idx], start, len);
            m_item_list[idx][len]  = '\0';
            idx++;

            start = pos + 1;
        }

        pos++;
    }

    uint32_t len = pos - start;
    if(len != 0)
    {
        m_item_list[idx] = new char [len + 1];
        strncpy(m_item_list[idx], start, len);
        m_item_list[idx][len]  = '\0';
    }
}

CStrExplode::~CStrExplode()
{
    for (uint32_t i = 0; i < m_item_cnt; i++) {
        delete [] m_item_list[i];
    }

    delete [] m_item_list;
}

#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <pthread.h>
#include "Lock.h"

class CCondition
{
public:
    CCondition(CLock* pLock);
    ~CCondition();
    void wait();
    /*
     * nWaitTime ms
     * if recv a signal then return true;
     * else return false;
     */
    bool waitTime(uint64_t nWaitTime);
    void notify();
    void notifyAll();
private:
    CLock* m_pLock;
    pthread_cond_t m_cond;
};

#endif /*defined(__CONDITION_H__) */

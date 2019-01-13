#ifndef __GT_LOCK_H_
#define __GT_LOCK_H_
#include "common.h"
#include "pthread.h"
class TPMutexLock
{
public:
	TPMutexLock();
	~TPMutexLock();
public:
	void Lock();
	void UnLock();
private:
	pthread_mutex_t m_mutexLock;
};

class TPGuard
{
public:
	TPGuard(TPMutexLock&);
	~TPGuard();
private:
	TPMutexLock& m_lock;
};

#endif
#include "tp_lock.h"

TPMutexLock::TPMutexLock()
{
	int err = pthread_mutex_init(&m_mutexLock, NULL);
	__ASSERT_EQUAL(err, 0);
}

TPMutexLock::~TPMutexLock()
{
	int err = pthread_mutex_destroy(&m_mutexLock);
	__ASSERT_EQUAL(err, 0);
}

void TPMutexLock::Lock()
{
	int err = pthread_mutex_lock(&m_mutexLock);
	if (err != 0)
	{
		__ASSERT_EQUAL(err, 0);
	}
}

void TPMutexLock::UnLock()
{
	int err = pthread_mutex_unlock(&m_mutexLock);
	if (err != 0)
	{
		__ASSERT_EQUAL(err, 0);
	}
}

TPGuard::TPGuard(TPMutexLock& lock) :m_lock(lock)
{
	m_lock.Lock();
}

TPGuard::~TPGuard()
{
	m_lock.UnLock();
}


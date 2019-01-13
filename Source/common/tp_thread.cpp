#include "tp_thread.h"

int TPThreadUtil::CreateThread(fpn_thread_func thread_func, void *param, /*[out]*/pthread_t *_tid)
{
	int err = 0;
	pthread_t tid = { 0 };
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);

	__ASSERT_EQUAL(err, 0);
	if (err != 0) return -1;

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	__ASSERT_EQUAL(err, 0);
	if (err != 0) return err;

	err = pthread_create(&tid, &attr, thread_func, param);
	__ASSERT_EQUAL(err, 0);
	if (err != 0)
	{
		__DEBUG_LOG("pthread_create err:%d, errno:%d:%s", err, errno, strerror(errno));
		return -1;
	}

	err = pthread_attr_destroy(&attr);
	__ASSERT_EQUAL(err, 0);
	if (err != 0) return -1;

	if (_tid != NULL)
	{
		*_tid = tid;
	}

	return 0;
}

int TPThreadUtil::CreateThread(fpn_thread_func thread_func, void *param, /*[out]*/pthread_t *_tid, int stacksize)
{
	int err = 0;

	pthread_t tid = { 0 };
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	__ASSERT_EQUAL(err, 0);
	if (err != 0) return -1;

	pthread_attr_setstacksize(&attr, stacksize);

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	__ASSERT_EQUAL(err, 0);
	if (err != 0) return err;

	err = pthread_create(&tid, &attr, thread_func, param);
	__ASSERT_EQUAL(err, 0);
	if (err != 0)
	{
		__DEBUG_LOG("pthread_create err:%d, errno:%d:%s", err, errno, strerror(errno));
		return -1;
	}

	err = pthread_attr_destroy(&attr);
	__ASSERT_EQUAL(err, 0);
	if (err != 0) return -1;

	if (_tid != NULL)
	{
		*_tid = tid;
	}

	return 0;
}

TPThread::TPThread() :m_IsRunning(false), m_NeedStop(false)
{

}

TPThread::~TPThread()
{

}

int TPThread::Start()
{
	if (!m_IsRunning)
	{
		TPGuard guard(m_BaseLock);

		if (!m_IsRunning)
		{
			m_IsRunning = true;
			m_NeedStop = false;

			return TPThreadUtil::CreateThread(TPThread::thread_func, this, NULL);
		}
	}
	return 0;
}

void TPThread::Stop()
{
	TPGuard guard(m_BaseLock);
	m_NeedStop = true;
}

bool TPThread::IsRunning() const
{
	return m_IsRunning;
}

void *TPThread::thread_func(void *param)
{
	__ASSERT(param != NULL);

	TPThread* This = (TPThread *)param;
	This->DoRun();
	This->m_IsRunning = false;

	return NULL;
}
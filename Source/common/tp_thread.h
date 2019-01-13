#ifndef __TP_THREAD_H__
#define __TP_THREAD_H__

#include "pthread.h"
#include "tp_lock.h"

typedef void* (*fpn_thread_func)(void *param);
class TPThreadUtil
{
public:
	static int CreateThread(fpn_thread_func thread_func, void *param,
		/*[out]*/pthread_t *tid);
	static int CreateThread(fpn_thread_func thread_func, void *param,
		/*[out]*/pthread_t *tid, int stacksize);
};

class TPThread
{
public:
	TPThread();
	virtual ~TPThread();

public:
	int Start();
	void Stop();
	bool IsRunning() const;

public:
	virtual void DoRun() = 0;

private:
	static void *thread_func(void *param);

protected:
	bool m_IsRunning;
	bool m_NeedStop;

	TPMutexLock m_BaseLock;
};

#endif // !__TP_THREAD_H__
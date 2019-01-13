#include "tp_sem.h"
int tp_sem_init(sem_t *sem)
{
	return sem_init(sem, 0, 0);
}

int tp_sem_getvalue(sem_t *sem, int *value)
{
	return sem_getvalue(sem, value);
}

int tp_sem_wait(sem_t *sem)
{
	return sem_wait(sem);
}

int tp_sem_post(sem_t *sem)
{
	return sem_post(sem);
}

int tp_sem_close(sem_t *sem)
{
	return sem_close(sem);
}

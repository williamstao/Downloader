#ifndef __TP_SEM_H__
#define __TP_SEM_H__
#include "common.h"
#include "semaphore.h"

int tp_sem_init(sem_t* sem);
int tp_sem_getvalue(sem_t* sem, int* value);
int tp_sem_wait(sem_t* sem);
int tp_sem_post(sem_t* sem);
int tp_sem_close(sem_t* sem);

#endif // !__TP_SEM_H__

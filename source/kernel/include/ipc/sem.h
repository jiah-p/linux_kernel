#ifndef __SEM_H__
#define __SEM_H__

#include "comm/types.h"
#include "tools/list.h"

// 进程同步： 信号量 锁 
typedef struct _sem_t{
    int count;
    list_t wait_list;
}sem_t;

void sem_init(sem_t * sem, int init_count);
void sem_wait(sem_t * sem);
void sem_notify(sem_t * sem);
int sem_count(sem_t * sem);

#endif
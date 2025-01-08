#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "tools/list.h"
#include "core/task.h"

typedef struct _mutex_t
{
    task_t * owner;
    int locker_count;               // 上锁次数
    list_t wait_list;
}mutex_t;

void mutex_init(mutex_t * mutex);


void mutex_lock(mutex_t * mutex);                   // 加锁
void mutex_unlock(mutex_t * mutex);                 // 解锁

#endif
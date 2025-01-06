#ifndef __TASK_H__
#define __TASK_H__

#include "cpu/cpu.h"
#include "comm/types.h"

// 任务相关 PCB 设计    目前只放一个 tss 结构
typedef struct _task_t
{
    tss_t tss;
}task_t;

// 任务初始化 参数：入口地址 和 栈顶指针
int task_init(task_t * task, uint32_t entry, uint32_t esp);

#endif
#ifndef __TASK_H__
#define __TASK_H__

#include "cpu/cpu.h"
#include "comm/types.h"

// 任务相关 PCB 设计    目前只放一个 tss 结构
typedef struct _task_t
{
    tss_t tss;
    int tss_sel;
    // 手动切换 依赖栈空间进行变量(寄存器)存储
    //  uint32_t * stack;
}task_t;

// 任务初始化 参数：入口地址 和 栈顶指针
int task_init(task_t * task, uint32_t entry, uint32_t esp);

// 任务切换
void task_switch_from_to(task_t * from, task_t * to);

#endif
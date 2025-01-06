#include "include/core/task.h"
#include "include/tools/klib.h"
#include "include/os_cfg.h"

static int tss_init(task_t * task, uint32_t entry, uint32_t ){
    kernel_memset(&task->tss, 0, sizeof(task_t));
    // eip 存放 当前执行指令的地址 ：初始化时 指向程序入口地址
    task->tss.eip = entry;
    // esp 程序栈顶指针 (注意程序各自栈的隔离)       特权级0 同值
    task->tss.esp = task->tss.eps0 = esp;

    // 选择子
    task->tss.ss = task->tss.ss0 = KERNEL_SELECTOR_DS;          // 平坦模型
    
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS;
    task->tss.cs = KERNEL_SELECTOR_CS;

    task->tss.eflags = EFLAGS_IF | EFLAGS_DEFAULT;
    return 0;
}

int task_init(task_t * task, uint32_t entry, uint32_t esp){
    ASSERT(task != (task_t * )0);

    tss_init(task, entry, esp);

    return 0;
}
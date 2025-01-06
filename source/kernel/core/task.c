#include "include/core/task.h"
#include "include/tools/klib.h"
#include "include/os_cfg.h"
#include "include/tools/log.h"

static int tss_init(task_t * task, uint32_t entry, uint32_t ){
    int tss_sel = gdt_alloc_desc();
    if(tss_sel == -1){
        log_print("alloc tss failded.");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
        SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS 
    );

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
    // 保存段选择子
    task->tss_sel = tss_sel;

    return 0;
}

int task_init(task_t * task, uint32_t entry, uint32_t esp){
    ASSERT(task != (task_t * )0);

    tss_init(task, entry, esp);
    // uint32_t * pesp = (uint32_t *)esp;
    // edi esi ebx ebp 寄存器初始化
    // if(pesp){
    //     *(--pesp) = entry;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     task->stack = pesp;
    // }

    return 0;
}

// 基于Linux任务切换机制 任务切换程序  参考：abi386-4.odf 
void simple_switch(uint32_t ** from, uint32_t  * to);

// 调用 硬件进行任务切换 参考卷3 实际硬件层面执行汇编的指令比较多
void task_switch_from_to(task_t * from, task_t * to){
    switch_to_tss(to->tss_sel);

    // 手动进行任务切换 需要保存内容：EBX EBP ESP EDI ESI
    // simple_switch(from->stack, to->stack);
}


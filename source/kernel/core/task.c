#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"


static task_mananger_t task_mananger;

static uint32_t idle_task_stack[IDLE_TASK_SIZE];          // 空闲进程栈空间

static int tss_init(task_t * task, uint32_t entry, uint32_t esp){
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

void task_set_ready(task_t * task){
    if(task == &task_mananger.idle_task) return;
    list_insert_first(&task_mananger.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block(task_t * task){
    if(task == &task_mananger.idle_task) return;
    list_remove(&task_mananger.ready_list, &task->run_node);
}

int task_init(task_t * task, const char * name, uint32_t entry, uint32_t esp){
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

    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->sleep_ticks = 0;
    task->slice_ticks = task->time_ticks;

    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    list_node_init(&task->wait_node);

    irq_state_t state = irq_enter_protection();
    task_set_ready(task);

    // 插入到 task_list
    list_insert_last(&task_mananger.task_list, &task->all_node);
    irq_leave_protection(state);

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

task_t * task_first_task(void){
    return &task_mananger.first_task;
}

void task_first_init(void){
    task_init(&task_mananger.first_task, "first task",0, 0);
    write_tr(task_mananger.first_task.tss_sel);

    task_mananger.curr_task = &task_mananger.first_task;
}

static void idle_task_entry(void){
    for(;;){
        hlt();
    }
}


// 任务管理模块初始化
void task_mananger_init(void){
    list_init(&task_mananger.ready_list);
    list_init(&task_mananger.task_list);
    list_init(&task_mananger.sleep_list);

    task_mananger.curr_task = (task_t *)0;
    task_init(&task_mananger.idle_task, "idle task", (uint32_t)idle_task_entry, (uint32_t)(idle_task_stack + IDLE_TASK_SIZE));

    task_first_init();
}

// 获取当前进程
task_t * task_current(void){
    return &task_mananger.curr_task;
}

int sys_sched_yield(void){
    irq_state_t state = irq_enter_protection();

    if(list_count(&task_mananger.ready_list) > 1){
        task_t * curr_task = task_current();

        task_set_block(curr_task);
        task_set_ready(curr_task);

        // 释放 cpu 使用权
        task_dispatch();
    }

    irq_leave_protection(state);

    return 0;
}

task_t * task_next_run(void){
    // 当空闲队列为空的时候  返回空闲进程
    if(list_count(&task_mananger.ready_list) == 0){
        return &task_mananger.idle_task;
    }

    list_node_t * task_node = list_first(&task_mananger.ready_list);

    return list_node_parent(task_node, task_t, run_node);
}



void task_dispatch(void){

    irq_state_t state = irq_enter_protection();

    // cpu调度策略
    task_t * to = task_next_run();
    if(to != task_mananger.curr_task){
        task_t * from = task_current();
        from->state = TASK_RUNNING;
        // 从 当前任务切换到 下一个就绪队列头部的任务，目前是 FIFO 算法
        task_switch_from_to(from, to);
    }

    irq_leave_protection(state);
}   

// 任务定时器  
void task_time_tick(void){
    task_t * curr_task = task_current();

    if(-curr_task->slice_ticks == 0){
        curr_task->slice_ticks = curr_task->time_ticks;
        
        task_set_block(curr_task);
        task_set_ready(curr_task);
        
        task_dispatch();
    }

    list_node_t  * curr = list_first(&task_mananger.sleep_list);
    while(curr){
        list_node_t * next = list_node_next(curr);

        task_t * task = list_node_parent(curr, task_t, run_node);
        if(--task->sleep_ticks == 0){
            task_set_wakeup(task);
            task_set_ready(task);
        }

        curr = next;
    }

    task_dispatch();
}

void task_set_sleep(task_t * task, uint32_t ticks){
    if(ticks <= 0 ) return;

    task->sleep_ticks = ticks;
    task->state = TASK_SLEEP;

    list_insert_last(&task_mananger.sleep_list, &task->run_node);
}

void task_set_wakeup(task_t * task){
    list_remove(&task_mananger.sleep_list, &task->run_node);
}

void sys_sleep(uint32_t ms){
    irq_state_t state = irq_enter_protection();

    task_set_block(task_mananger.curr_task);
    task_set_sleep(task_mananger.curr_task, (ms + (OS_TICKS_MS - 1))  / OS_TICKS_MS );              //向上取整
    // 任务切换
    task_dispatch();

    irq_leave_protection(state);
}   

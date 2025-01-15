#include "core/task.h"
#include "tools/klib.h"
#include "os_cfg.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "cpu/mmu.h"
#include "core/memory.h"
#include "core/syscall.h"

static task_mananger_t task_mananger;

// task table 
static task_t task_table[TASK_NR];
// task table mutex
static mutex_t task_table_mutex;

static uint32_t idle_task_stack[IDLE_TASK_SIZE];          // 空闲进程栈空间

static int tss_init(task_t * task, int flag, uint32_t entry, uint32_t esp){
    int tss_sel = gdt_alloc_desc();
    if(tss_sel == -1){
        log_print("alloc tss failded.");
        return -1;
    }

    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
        SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS 
    );

    // 给 特权级0 分配内存
    uint32_t kernel_stack = memory_alloc_page();
    if(kernel_stack == 0){
        goto tss_init_failed;
    }

    kernel_memset(&task->tss, 0, sizeof(task_t));
    // eip 存放 当前执行指令的地址 ：初始化时 指向程序入口地址
    task->tss.eip = entry;
    // esp 程序栈顶指针 (注意程序各自栈的隔离)       特权级0 同值
    task->tss.esp = esp;
    task->tss.eps0 = kernel_stack + MEM_PAGE_SIZE;          // 指向高地址 注意：栈生长方向

    // 选择子
    int code_sel, data_sel;
    if(flag & TASK_FLAGS_SYSTEM){
        code_sel = KERNEL_STACK_SIZE;
        data_sel = KERNEL_SELECTOR_DS;
    }else{
        code_sel = task_mananger.app_code_sel | SEG_CPL3;
        data_sel = task_mananger.app_data_sel | SEG_CPL3;
    }
  
    task->tss.ss = data_sel;          
    task->tss.ss0 = KERNEL_SELECTOR_DS;     // 特权级 0 的时候 栈描述符
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = data_sel;
    task->tss.cs = KERNEL_SELECTOR_CS;

    task->tss.eflags = EFLAGS_IF | EFLAGS_DEFAULT;
    // 保存段选择子
    task->tss_sel = tss_sel;
    // 创建进程的页表
    uint32_t page_dir = memory_create_uvm();

    if(page_dir == 0){
        
        goto  tss_init_failed;
    }
    task->tss.cr3 = page_dir;
    task->tss_sel = tss_sel;
    return 0;

// 资源回收
tss_init_failed:
    gdt_free_sel(tss_sel);
    if(kernel_stack){       
        memory_free_page(kernel_stack);
    }
    return -1;
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

int task_init(task_t * task, const char * name, int flag, uint32_t entry, uint32_t esp){
    ASSERT(task != (task_t * )0);
    
    tss_init(task, flag, entry, esp);
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
    task->parent = (task_t *)0;
    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    list_node_init(&task->wait_node);

    irq_state_t state = irq_enter_protection();
    task_set_ready(task);

    task->pid = (uint32_t)task;         // 也可以用个 static 值进行累加

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
    void first_task_entry(void);
    extern uint8_t s_first_task[], e_first_task[];

    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task);
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    ASSERT(copy_size < alloc_size);

    uint32_t first_start = (uint32_t)first_task_entry;              // 0x80000000 first_task_entry

    task_init(&task_mananger.first_task, "first task", 0, first_start, first_start + alloc_size);          // flag 0 -> dpl3 
    write_tr(task_mananger.first_task.tss_sel);

    task_mananger.curr_task = &task_mananger.first_task;

    // 任务页表的切换
    mmu_set_page_dir(task_mananger.first_task.tss.cr3);


    memory_alloc_page_for(first_task_entry, alloc_size, PTE_P | PTE_W | PTE_U);

    kernel_memcpy((void *)first_start, (void *)s_first_task, copy_size);
    
}

static void idle_task_entry(void){
    for(;;){
        hlt();
    }
}


// 任务管理模块初始化
void task_mananger_init(void){
    kernel_memset(task_table, 0, sizeof(task_table));
    mutex_init(&task_table_mutex);

    int sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x000000000, 0xFFFFFFFF, SEG_P_PRESENT | SEG_DPL3 | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);

    task_mananger.app_data_sel = sel;

    sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x000000000, 0xFFFFFFFF, SEG_P_PRESENT | SEG_DPL3 | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    task_mananger.app_code_sel = sel;

    list_init(&task_mananger.ready_list);
    list_init(&task_mananger.task_list);
    list_init(&task_mananger.sleep_list);

    task_mananger.curr_task = (task_t *)0;
    // 空闲任务 特权级 设置为 TASK_FLAGS_SYSTEM (1 << 0)
    task_init(&task_mananger.idle_task, "idle task", TASK_FLAGS_SYSTEM, (uint32_t)idle_task_entry, (uint32_t)(idle_task_stack + IDLE_TASK_SIZE));

    task_first_init();
}

// 获取当前进程
task_t * task_current(void){
    return task_mananger.curr_task;
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

int sys_getpid(void){
    task_t * task = task_current();

    return task->pid;
}

// task table alloc 
static task_t * alloc_task(void){

    task_t * task = (task_t *) 0;
    
    for(int i = 0; i < TASK_NR; i++){
        task_t * cur = task_table + i;
        if(cur->name[0] = '\0'){
            task = cur;
            break;
        }
        
    }
    return task;
}

// task table release
static void release_task(task_t * task){
    mutex_lock(&task_table_mutex);

    task->name[0] = '\0';

    mutex_unlock(&task_table_mutex);
}

void task_uninit(task_t * task){
    if(task->tss_sel){
        gdt_free_sel(task->tss_sel);
    }
    if(task->tss.eps0){
        memory_free_page(task->tss.esp - MEM_PAGE_SIZE);
    }

    if(task->tss.cr3){
        memory_destory_uvm(task->tss.cr3);
    }

    kernel_memset(task, 0, sizeof(task_t));
}

void free_task(task_t * task){

}


int sys_fork(void){
    task_t * parent_task = task_current();

    task_t * child_task = alloc_task();

    if(child_task == (task_t * ) 0){
        goto fork_failed;
    }

    syscall_frame_t * frame =  (syscall_frame_t *)(parent_task->tss.eps0 - sizeof(syscall_frame_t));
    // 注意入口地址 和 esp 设置
    int err = task_init(child_task, parent_task->name, 0, frame->eip, frame->esp + sizeof(uint32_t) * SYSCALL_PARAM_COUNT);
    if(err < 0){
        goto fork_failed;
    }

    // 
    tss_t * tss = &child_task->tss;
    tss->eax = 0;
    tss->ebx = frame->ebx;
    tss->ecx = frame->ecx;
    tss->edx = frame->edx;
    tss->esi = frame->esi;
    tss->ebp = frame->ebp;

    tss->cs = frame->cs;
    tss->ds = frame->ds;
    tss->es = frame->es;
    tss->fs = frame->fs;
    tss->gs = frame->gs;
    tss->eflags = frame->eflags;

    child_task->parent = parent_task;

    if((tss->cr3 == memory_copy_uvm(parent_task->tss.cr3)) < 0){
        goto fork_failed;
    }

    return child_task->pid;
    
fork_failed:
    // 分配失败
    if(child_task){
        task_uninit(child_task);
        free_task(child_task);

    }

    return -1;
}


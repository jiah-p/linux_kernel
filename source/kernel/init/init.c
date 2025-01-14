#include "init.h"
#include "comm/boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "comm/cpu_instr.h"
#include "tools/log.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "core/task.h"
#include "tools/list.h"
#include "ipc/sem.h"
#include "ipc/mutex.h"
#include "core/memory.h"

// test
// int global_var = 0x1234;
// int globaL_var_zero;
// static int static_global_var = 0x2345;
// static int static_global_var_zero;

// const int const_int = 0x23;
// const char * str = "abcdefg";


void kernel_init(boot_info_t * boot_info){
    //    test
    //    int local_var;
    //    static int static_local_var = 0x23;
    //    static int static_local_var_zero;

    // assert 断言调试
    // ASSERT(boot_info->ram_region_count != 0);

    cpu_init();
    log_init();

    memory_init(boot_info);

    irq_init();
    time_init();
}



// static uint32_t init_task_stack[1024];      // init_task_entry 栈空间
// static task_t init_task;
// static sem_t sem;                           // 全局信号量

// void init_task_entry(void){
//     int count = 0;
//     for (;;)
//     {
//         sem_wait(&sem);                     // 信号量同步
//         log_print("Int task: %d", count++);
//         // 任务切换
//         // task_switch_from_to(&init_task, task_first_task());

//         // sys_sched_yield();              // 释放当前cpu资源

//         // sys_sleep(1000);                // 延时 1 s

//         // 通过信号量进行进程间通信的测试

//     }
// }   

// 链表 test
void list_test(){
    list_t list;
    list_init(&list);

    // 宏定义 list_node_parent 测试
    struct type_t
    {
        int i;
        list_node_t node;
    }v ={0x123456};

    list_node_t * v_node = &v.node;

    struct type_t * a = (struct type_t *)0;         // 平坦模型 直接指针指向 0地址处
    uint32_t addr = (uint32_t)&a->node;             // int 类型占 4个字节， 所以 addr = 4
    uint32_t addr_p = offset_in_parent(struct type_t, node);    // 同上
    // 得到结构体的地址(指针)
    struct type_t * p = list_node_parent(v_node, struct type_t, node);
    if(p->i == 0x123456){
        log_print("transfer correctly.");
    }else{
        log_print("transfer error.");
    }
}

// 跳转进入 first_task 任务
void move_to_first_task(void){
    task_t *  curr = task_current();
    ASSERT(curr != 0);

    tss_t * tss = &(curr->tss);
    // 内联汇编进行跳转，手动压入 各个寄存器值，iret自动返回到各个寄存器
    __asm__ __volatile__(
        "push %[ss]\n\t"
        "push %[esp]\n\t"
        "push %[eflags]\n\t"
        "push %[cs]\n\t"
        "push %[eip]\n\t"
        // 返回 低特权级栈
        "iret"::[ss]"r"(tss->ss),
        [esp]"r"(tss->esp),
        [eflags]"r"(tss->eflags),
        [cs]"r"(tss->cs),
        [eip]"r"(tss->eip)
        
    );
}

void init_main(){
    
    list_test();

    log_print("kernel is already.");
    // 字符串测试
    log_print("Vers on: %s", OS_VERSION);
    // 整数测试
    log_print("%d %d %x %c", -123, 345, 0x12ff, 'a');

    // 除 0 异常测试
    int  i = 3 / 0;
    
    // 打开全局中断，开中断
    // irq_enable_global();

    // task (PCB) 初始化
    // task_init(&init_task, "init task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);         // 栈 高地址 -> 低地址 所以压入  init_task_stack[1024] 应该是最高位 小端存储 // 而数据永远从低位开始读取
    task_first_init();

    move_to_first_task();

    // 更改 tr寄存器
    // write_tr(first_task.tss_sel);

    // 全局信号量初始化
    // sem_init(&sem, 0);

    // // 打开全局中断，定时器开始工作
    // irq_enable_global();
    // // 任务1 
    // int count = 0;
    // for(;;){
    //     log_print("Count: %d", count++);
    //     // 任务切换 任务2 
    //     // task_switch_from_to(task_first_task(), &init_task);

    //     // 主动switch -> 主动放弃cpu资源 通过任务管理 由cpu进行调度
    //     // sys_sched_yield();

    //     // 信号量发送
    //     sem_notify(&sem);

    //     // 设计延时
    //     sys_sleep(1000);
    // }
}


#include "init.h"
#include "comm/boot_info.h"
#include "include/cpu/cpu.h"
#include "include/cpu/irq.h"
#include "dev/time.h"
#include "comm/cpu_instr.h"
#include "include/tools/log.h"
#include "include/os_cfg.h"
#include "tools/klib.h"
#include "include/core/task.h"
#include "include/tools/list.h"

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
    ASSERT(boot_info->ram_region_count != 0);

    cpu_init();

    log_init();
    irq_init();
    time_init();
}


static task_t first_task;                   // init_task_entry
static uint32_t init_task_stack[1024];      // init_task_entry 栈空间
static task_t init_task;


void init_task_entry(void){
    int count = 0;
    for (;;)
    {
        log_print("Int task: %d", count++);
        // 任务切换
        task_switch_from_to(&init_task, &first_task);
    }
    
}   

// 链表 test
void list_test(){
    list_t list;
    list_init(&list);
}

void init_main(){
    
    list_test();

    log_print("kernel is already.");
    // 字符串测试
    log_print("Vers on: %s", OS_VERSION);
    // 整数测试
    log_print("%d %d %x %c", -123, 345, 0x12ff, 'a')

    // 除 0 异常测试
    int  i = 3 / 0;
    
    // 打开全局中断，开中断
    // irq_enable_global();

    // task (PCB) 初始化
    task_init(&init_task, (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);         // 栈 高地址 -> 低地址 所以压入  init_task_stack[1024] 应该是最高位 小端存储 // 而数据永远从低位开始读取
    task_init(&first_task, (uint32_t)0, (uint32_t)0);

    // 更改 tr寄存器
    write_tr(first_task.tss_sel);


    

    // 任务1 
    int count = 0;
    for(;;){
        log_print("Count: %d", count++);
        // 任务切换 任务2 
        task_switch_from_to(&first_task, &init_task);
    }

}


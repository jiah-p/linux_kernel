#include "init.h"
#include "comm/boot_info.h"
#include "include/cpu/cpu.h"
#include "include/cpu/irq.h"
#include "dev/time.h"
#include "include/tools/log.h"
#include "include/os_cfg.h"
#include "tools/klib.h"

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

void init_task_entry(void){
    int count = 0;
    for (;;)
    {
        log_print("Int task: %d", count++);
    }
    
}   

void init_main(){

    log_print("kernel is already.");
    // 字符串测试
    log_print("Vers on: %s", OS_VERSION);
    // 整数测试
    log_print("%d %d %x %c", -123, 345, 0x12ff, 'a')

    // 除 0 异常测试
    int  i = 3 / 0;
    
    // 打开全局中断，开中断
    // irq_enable_global();


    // 任务1 
    int count = 0;
    for(;;){
        log_print("Count: %d", count++);
    }
    // 任务2 
    init_task_entry();
}
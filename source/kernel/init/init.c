#include "init.h"
#include "comm/boot_info.h"
#include "include/cpu/cpu.h"
#include "include/cpu/irq.h"
#include "dev/time.h"
#include "include/tools/log.h"
#include "include/os_cfg.h"
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

    cpu_init();
    log_init();
    irq_init();
    time_init();
    
}

void init_main(){

    log_print("kernel is already.");
    log_print("Version: %s", OS_VERSION);
    // 打开全局中断，开中断
    // irq_enable_global();
    for(;;){}
}
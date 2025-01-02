#include "init.h"
#include "comm/boot_info.h"
#include "include/cpu/cpu.h"
#include "include/cpu/irq.h"

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
    irq_init();
}

void init_main(){
    for(;;){}
}
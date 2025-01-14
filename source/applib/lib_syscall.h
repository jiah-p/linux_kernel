#ifndef __LIB_SYSCALL__
#define __LIB_SYSCALL__

#include "os_cfg.h"

#define SYS_SLEEP           0

typedef struct _syscall_args_t
{
    // 对应调用门 类型
    int id;
    //  4 param counts
    int arg0;
    int arg1;
    int arg2;
    int arg3;
}syscall_args_t;

static inline int syscall(syscall_args_t * args){
    uint32_t addr[] = {0, SELECTOR_SYSCAL | 0};

    __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcall *(%[a])"
        ::[arg3]"r"(args->arg3),
        [arg2]"r"(args->arg2),
        [arg1]"r"(args->arg1),
        [arg0]"r"(args->arg0),
        [id]"r"(args->id),
        [a]"r"(addr)
    );

}

static inline void msleep(int ms){
    if(ms <= 0){
        return 0;
    }

    syscall_args_t args;
    // 设置参数
    args.id = SYS_SLEEP;
    args.arg0 = ms;

    // 通过调用门进行实现
    return sys_call(&args); 
    
}

#endif

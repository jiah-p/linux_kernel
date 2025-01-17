#ifndef __LIB_SYSCALL__
#define __LIB_SYSCALL__

#include "os_cfg.h"
#include "comm./types.h"

#define SYS_SLEEP           0
#define SYS_GETPID          1
#define SYS_FORK            2
#define SYS_EXECVE          3
#define SYS_PRINTMSG        100

#define SYS_PARAM_COUNT     5

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
    int ret;

    __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcall *(%[a])"
        :"=a"(ret)
        :[arg3]"r"(args->arg3),
        [arg2]"r"(args->arg2),
        [arg1]"r"(args->arg1),
        [arg0]"r"(args->arg0),
        [id]"r"(args->id),
        [a]"r"(addr)
    );

    return ret;

}

static inline int msleep(int ms){
    if(ms <= 0){
        return 0;
    }

    syscall_args_t args;
    // 设置参数
    args.id = SYS_SLEEP;
    args.arg0 = ms;

    // 通过调用门进行实现
    return syscall(&args); 
    
}

static inline int getpid(void){
    syscall_args_t args;
    // 设置参数
    args.id = SYS_GETPID;

    // 通过调用门进行实现
    return syscall(&args); 
}


static inline void print_msg(const char * fmt, int args){
    syscall_args_t arg;

    arg.id = SYS_PRINTMSG;
    arg.arg0 = (int)fmt;

    arg.arg1 = args;

    syscall(&arg);
}

static inline int fork(void){
    syscall_args_t arg;

    arg.id = SYS_FORK;

    syscall(&arg);
}

static inline int execve(const char * name, char * const * argv, char * const * env){
    syscall_args_t arg;

    arg.id = SYS_EXECVE;
    arg.arg0 = (int)name;

    arg.arg1 = argv;
    arg.arg2 = env;

    syscall(&arg);
}

#endif

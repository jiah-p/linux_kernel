#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "comm/types.h"

#define SYSCALL_PARAM_COUNT         5

void exception_handler_syscall(void);

// 调用门 压栈模拟的结构体
typedef struct _syscall_frame_t
{   
    int eflags;
    int gs, fs, es, ds;
    uint32_t edi, esi, ebp, dummy, ebx, edx, ecx, eax;        // pusha 指令压入的寄存器内容
    int eip, cs;
    int func_id, arg0, arg1, arg2, arg3;   
    int esp, ss;                        
}syscall_frame_t;



#endif
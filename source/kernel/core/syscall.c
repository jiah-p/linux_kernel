#include "core/syscall.h"
#include "lib_syscall.h"
#include "core/task.h"
#include "tools/log.h"

typedef int (*syscall_hanler_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

void sys_printmsg(const char * fmt, int arg){
    log_print(fmt, arg);
}

// 转发表[系统调用]句柄
static const sys_handler_t sys_table[] = {
    [SYS_SLEEP] = (sys_handler_t)sys_sleep, 
    [SYS_GETPID] = (sys_handler_t)sys_getpid,
    [SYS_PRINTMSG] = (sys_handler_t)sys_printmsg;
    [SYS_FORK] = (sys_handler_t)sys_fork;
};


// 系统调用接口
void do_handler_syscall(syscall_frame_t * frame){
    if(frame->func_id < sizeof(sys_table) / sizeof(sys_handler_t)){
        syscall_hanler_t handler = sys_table[frame->func_id];
        if(handler){
            // 返回值 本质 也是将值放到 eax寄存器中，然后再赋值给 ret 对象
            /* 
                return task->pid 的反汇编
                mov -0x4(%ebp), %eax
                mov 0x4(%eax), %eax
            */
            int ret = handler(arg0, arg1, arg2, arg3);     
            // 将 ret 返回出去
            frame->eax = ret;

            return;
        }

    }

    task_t * task = task_current();
    log_print("task: %s, Unknown syscall: %d", task->name, frame->func_id);
}


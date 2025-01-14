#include "core/syscall.h"
#include "lib_syscall.h"
#include "core/task.h"
#include "tools/log.h"

typedef int (*syscall_hanler_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

// 转发表[系统调用]句柄
static const sys_handler_t sys_table[] = {
    [SYS_SLEEP] = sys_sleep, 
};


// 系统调用接口
void do_handler_syscall(syscall_frame_t * frame){
    if(frame->func_id < sizeof(sys_table) / sizeof(sys_handler_t)){
        syscall_hanler_t handler = sys_table[frame->func_id];
        if(handler){
            int ret = handler(arg0, arg1, arg2, arg3);
            return;
        }

    }

    task_t * task = task_current();
    log_print("task: %s, Unknown syscall: %d", task->name, frame->func_id);
}


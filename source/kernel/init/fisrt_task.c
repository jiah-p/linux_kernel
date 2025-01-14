#include "core/task.h"
#include "tools/log.h"
#include "applib/lib_syscall.h"

int first_task_main(void){
    for (;;)
    {   
        // 之后做内核接口 供进程进行调用
        // log_print("first task.");
        // sys_sleep(1000);

        msleep(1000);
    }
    


    return 0;
} 
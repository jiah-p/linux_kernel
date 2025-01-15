#include "core/task.h"
#include "tools/log.h"
#include "applib/lib_syscall.h"

// 基本test
// int first_task_main(void){
//     int pid = getpid();

//     for (;;)
//     {   
//         // 之后做内核接口 供进程进行调用
//         // log_print("first task.");
//         // sys_sleep(1000);

//         print_msg("task id = %d", pid);

//         msleep(1000);
//     }
    
//     return 0;
// } 

// fork调用 test
int first_task_main(){
    
    int count = 3;
    int pid = getpid();
    print_msg("first task pid: %d",pid);
    pid = fork();

    if(pid < 0)
        print_msg("error id: %d", pid);
    else if (pid == 0){
        print_msg("child fork id is %d", pid);
    }
    else{
        print_msg("child fork id is %d", pid);
        print_msg("parent: %d", count);
    }

    for (;;)
    {   
    }
    

}
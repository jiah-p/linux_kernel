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

    // 如果是相同的 页表机制的话，那么会导致 父进程的栈空间 被改动，会引发异常
    // 子进程 需要重新 分配栈空间，并建立 原本父进程的 页表映射 -> 自己空间内进行修改
    if(pid < 0)
        print_msg("error id: %d", pid);
    else if (pid == 0){
        // 进入 子进程
        print_msg("child fork id is %d", pid);

        char * argv[] = {"arg0", "arg1", "arg2", "arg3"};
        execve("/shell.elf", argv, (char **)0);
    }
    else{
        print_msg("child fork id is %d", pid);
        print_msg("parent: %d", count);
    }

    for (;;)
    {   
    }
    

}
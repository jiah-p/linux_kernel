#ifndef __TASK_H__
#define __TASK_H__

#include "cpu/cpu.h"
#include "comm/types.h"
#include "tools/list.h"

#define TASK_NAME_SIZE                  32
#define TASK_TIME_SLICE_DEFAULT         10
#define IDLE_TASK_SIZE                  102

// 任务类型的特权级
#define TASK_FLAGS_SYSTEM               (1 << 0)

// 任务相关 PCB 设计    目前只放一个 tss 结构
typedef struct _task_t
{
    enum{
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITTING   
    }state;     // 任务的状态

    int sleep_ticks;                // 延时时长
    int slice_ticks;
    int time_ticks;

    char  name[TASK_NAME_SIZE];     // 任务名称
    list_node_t run_node;           // 插入 ready_list
    list_node_t all_node;           // 插入 task_list
    list_node_t wait_node;          // 插入 wait_list

    tss_t tss;
    int tss_sel;
    //  uint32_t * stack;
}task_t;

// 任务初始化 参数：入口地址 和 栈顶指针
int task_init(task_t * task, const char * name,int flag, uint32_t entry, uint32_t esp);

// 任务切换
void task_switch_from_to(task_t * from, task_t * to);

void task_time_tick(void);

typedef struct _task_mananger_t{
    task_t * curr_task;             // 当前运行任务
    list_t ready_list;              // 就绪队列
    list_t task_list;               
    list_t sleep_list;              // 延时队列

    task_t first_task;    
    task_t idle_task;               // 空闲进程      

    int app_code_sel;
    int app_data_sel;       
}task_mananger_t;


void task_mananger_init(void);
void task_first_init(void);
task_t * task_first_task(void);

void task_set_ready(task_t * task);
void task_set_block(task_t * task);

// 进程 主动释放 cpu 资源
int sys_sched_yield(void);

task_t * task_next_run(void);
task_t * task_current(void);
void task_dispatch(void);

void task_set_sleep(task_t * task, uint32_t ticks);     //. 将任务插入到延时队列并设置延时时长
void task_set_wakeup(task_t * task);

void sys_sleep(uint32_t ms);


#endif
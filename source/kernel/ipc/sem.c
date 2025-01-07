#include "include/ipc/sem.h"
#include "include/core/task.h"
#include "include/cpu/irq.h"

void sem_init(sem_t * sem, int init_count){
    if(init_count < 0) return;
    sem->count = init_count;

    list_init(sem->wait_list);
}

// 编程模式：信号量结构体（信号量，该信号的等待队列）
// 申请信号量
void sem_wait(sem_t * sem){
    // 关中断进行保护
    irq_state_t state = irq_enter_protection();

    // 如果信号量为0 则进行 等待（阻塞，并进入等待队列），否则 更改信号量
    if(sem->count < 1){
        // 信号量等待，但 不改变信号量！！！
        task_t * curr = task_current();
        task_set_block(curr);
        list_insert_last(&sem->wait_list, &curr->wait_node);    
        task_dispatch();
  
    }else{
        sem->count--;
    }
   
    irq_leave_protection(state);

    return;
}

// 释放信号量
void sem_notify(sem_t * sem){
    // 关中断进行保护
    irq_state_t state = irq_enter_protection();
    // 如果有等待队列则安排 任务 进入 就绪 队列，并更改信号量
    if(list_count(&sem->wait_list)){
        list_node_t * node = list_remove_first();
        task_t * task = list_node_parent(node, task_t, wait_node);
        task_set_ready(task);

        task_dispatch();
    }else{
        sem->count++;
    }

    irq_leave_protection(state);

}
#include "include/ipc/mutex.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"

void mutex_init(mutex_t *  mutex){
    mutex->owner = (task_t *)0;
    mutex->locker_count = 0;

    list_init(&mutex->locker);
}

// 加锁
void mutex_lock(mutex_t * mutex){
    irq_state_t state = irq_enter_protection();

    task_t * curr = task_current();

    if(mutex->locker_count == 0){
        mutex->locker_count++;
        mutex->owner = curr;
    }else if(mutex->owner == curr){
        mutex->locker_count++;              // 当前进程重复上锁
    }else{
        task_set_block(curr);
        list_insert_last(&mutex->wait_list, &curr->wait_node);
        task_dispatch();
    }

    irq_leave_protection(state);
}

// 解锁
void mutex_unlock(mutex_t * mutex){
    irq_state_t state = irq_enter_protection();

    task_t * curr = task_current();

    // 确认解锁资格
    if(curr = mutex->owner){
        if(--mutex->locker_count == 0){
            mutex->owner = (task_t *)0;
            if(list_count(&mutex->wait_list)){
                list_node_t * node= list_remove_first(&mutex->wait_list);
                task_t * task = list_node_parent(node, task_t, wait_node);
                task_set_ready(task);

                mutex->locker_count = 1;
                mutex->owner = task;
            }
        }
    }

    irq_leave_protection(state);
}                 

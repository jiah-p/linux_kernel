#include "include/dev/time.h"
#include "comm/types.h"
#include "include/cpu/irq.h"
#include "comm/cpu_instr.h"
#include "include/os_cfg.h"
#include "include/core/task.h"

static uint32_t sys_tick;

void do_handler_time(exception_frame_t * frame){
    sys_tick++;

    // 通知 8259 中断处理函数已经完成
    pic_send_eoi(IRQ0_TIMER);
    // 任务定时器检查
    task_time_tick();
}

static void init_pit(void){ 
    // 1000 / 时钟节拍  = 每个时钟节拍的占多少S
    uint32_t reload_count  = OS_TICKS_MS / (1000.0 / PIT_OSC_FREQ); 

    outb(PIT_COMMAND_MODE_PORT, PIT_CHANNEL | PIT_LOAD_LOHI | PIT_MODE3);
    outb(PIT_CHANNEL0_DATA_PORT, reload_count & 0xff);
    outb(PIT_CHANNEL0_DATA_PORT, (reload_count >> 8) & 0xff);

    irq_install(IRQ0_TIMER,  (irq_handler_t)exception_handler_time);
    irq_enable(IRQ0_TIMER);
}

// 定时器初始化
void time_init(void){
    sys_tick = 0;

    init_pit();
}
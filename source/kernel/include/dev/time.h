#ifndef __TIME_H__
#define __TIME_H__

// 时钟源   时钟节拍
#define PIT_OSC_FREQ        1193182

// 数据和控制端口
#define PIT_COMMAND_MODE_PORT    0x43
#define PIT_CHANNEL0_DATA_PORT   0x40

#define PIT_CHANNEL             (0 << 6)
#define PIT_LOAD_LOHI           (3 << 4)
#define PIT_MODE3               (3 << 1)


void time_init(void);
// 中断处理函数
void exception_handler_time(void);

#endif
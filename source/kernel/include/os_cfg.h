#ifndef __OS_CFG__
#define __OS_CFG__


#define GDT_TABLE_SIZE          256
#define KERNEL_SELECTOR_CS      (1 * 8)
#define KERNEL_SELECTOR_DS      (2 * 8)
#define KERNEL_STACK_SIZE       (8 * 1024)

// 定时器中断间隔
#define OS_TICKS_MS             10

// 操作系统版本号
#define OS_VERSION              "1.0.0"

#endif 
#include "include/tools/log.h"
#include "include/os_cfg.h"
#include "comm/types.h"
#include "comm/cpu_instr.h"
#include "include/tools/klib.h"
#include "include/cpu/irq.h"
#include "include/ipc/mutex.h"

static mutex_t mutex;

#include COM1_PORT          0x3F8

void log_init(void){
    // 初始化 ipc 互斥锁
    mutex_init(&mutex);

    // 初始化 串行接口 serial0 
    outb(COM1_PORT + 1, 0x00);      // 关 串行接口 中断
    outb(COM1_PORT + 3, 0x80);      // 设置 传输速度
    outb(COM1_PORT + 0, 0x3);   
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xc7);
    outb(COM1_PORT + 1, 0x00);
}

void log_print(const char * fmt, ...){
    // fmt 及 可变参数进行处理
    char str_buf[128];
    kernel_memset(str_buf, 0, sizeof(str_buf));
    // gcc 宏
    va_list args;
    va_start(args, fmt);        // fmt 可变参数 -> args
    
    kernel_vsprint(str_buf, fmt, args);
    va_end(args);
    
    // 关中断 进入临界区
    mutex_lock(&mutex);

    const char * p = str_buf;

    while(*p != '\0'){
        // 检测 串行接口 5号寄存器第六位 是否忙
        while((inb(COM1_PORT + 5)) & (1 << 6) == 0);
        outb(COM1_PORT, *p++);
    }

    // 自动换行
    outb(COM1_PORT, '\r');          // \r 将 列号归0
    outb(COM1_PORT, '\n');          // \n 行号 + 1

    mutex_unlock(&mutex);
}
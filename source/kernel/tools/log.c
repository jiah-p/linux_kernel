#include "include/tools/log.h"
#include "include/os_cfg.h"
#include "comm/types.h"
#include "comm/cpu_instr.h"

#include COM1_PORT          0x3F8

void log_init(void){

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
    const char * p = fmt;

    while(*p != '\0'){
        // 检测 串行接口 5号寄存器第六位 是否忙
        while((inb(COM1_PORT + 5)) & (1 << 6) == 0);
        outb(COM1_PORT, *p++);
    }

    // 自动换行
    outb(COM1_PORT, '\r');          // \r 将 列号归0
    outb(COM1_PORT, '\n');          // \n 行号 + 1
}
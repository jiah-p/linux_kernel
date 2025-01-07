#ifndef __LOG_H__
#define __LOG_H__

// 初始化 日志系统
void log_init(void);

// 可变参数的 print 日志接口 调用 qemu 的 serial0 串行接口进行输出
void log_print(const char * fmt, ...);

#endif
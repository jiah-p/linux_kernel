#ifndef __KLIB_H__
#define __KLIB_H__

#include "stdarg.h"             // 处理可变参数
#include "comm/boot_info.h"
#include "comm/types.h"
// string 相关封装接口

// assert 宏定义调试
  
// 对齐 case: size = 0x1010 bound = 0x1000  -> 0xFFFFF000  & 0x1010  取 4K以上位
static inline uint32_t down2(uint32_t size, uint32_t bound){
    return size & ~(bound - 1);
}

// 向上取整
static inline uint32_t up2(uint32_t size, uint32_t bound){
    return (size + (bound - 1)) & ~(bound - 1);     
}

void kernel_strcpy(char * dest, const char * src);
void kernel_strncpy(char * dest, const char * src, int size);
int kernel_strcmp(const char * s1, const char * s2);
int kernel_strlen(const char * str);

void kernel_memcpy(void* dest, void * src, int size);
void kernel_memset(void * dest,uint8_t v, int size);
int kernel_memcmp(void * d1, void *d2, int size);

void kernel_itoa(char * buf, int num, int base);

void kernel_vsprint(char * buf, const char * fmt, va_list args);
void kernel_sprint(char * buf, const char * fmt, ...);

#ifndef RELEASE 
#define ASSERT(expr)    \
    if(!(expr))  pannic(__FILE__, __LINE__, __func__, #expr)      // 通过 # 会将expr进行替换成 字符串
void pannic(const char * file, int line, const char * func, const char * cond);

#else 
#define ASSERT(expr)        ((void)0)
#endif

#endif
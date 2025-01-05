#ifndef __KLIB_H__
#define __KLIB_H__

#include "comm/boot_info.h"
// string 相关封装接口

void kernel_strcpy(char * dest, const char * src);
void kernel_strncpy(char * dest, const char * src, int size);
int kernel_strcmp(const char * s1, const char * s2);
int kernel_strlen(const char * str);

void kernel_memcpy(void* dest, void * src, int size);
void kernel_memset(void * dest,uint8_t v, int size);
int kernel_memcpy(void * d1, void *d2, int size);


#endif
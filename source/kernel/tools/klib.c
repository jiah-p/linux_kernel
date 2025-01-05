#include "include/tools/klib.h"

// 拷贝字符串 末尾置 \0
void kernel_strcpy(char * dest, const char * src){
    if(!dest || !src) return ;

    while (*src)    // '\0' 的时候会退出，所以 dest 也要加 \0
    {       
        *dest++ = *src++;
    }
    
    *dest = '\0';
}

// 拷贝字符串前 n 个字符  末尾置 \0
void kernel_strncpy(char * dest, const char * src, int size){
    if(!dest || !src || size <= 0) return ;

    while (*src && size-- > 0)
    {       
        *dest++ = *src++;
    }
    
    // 如果 *src != '\0' 那么 size == 0 所以    *dest = '\0';
    *dest = '\0';
}

// 比较字符串前 n 个是否相等
int kernel_strncmp(const char * s1, const char * s2, int size){
    if(!s1 || !s2) return -1;

    uint32_t count;
    while(*s1 &&  (*s1 == *s2) && size--){
        s1++;
        s2++;
    }

    // 相等 个人定义：*s1 == *s2 完全一模一样才相等
    if(*s1 == *s2 && *s1 == '\0') return 0;
    if(*s1 == *s2 && size == 0 ) return 0;

    if(*s1 > *s2) return 1;
    else{
        return -1;
    }
}

// 返回 字符串 有效字符
int kernel_strlen(const char * str){
    if(!str) return 0;

    uint32_t  len = 0;

    while (*str++)
    {
       len++;
    }

    return len;
}

// 内存按数量进行拷贝
void kernel_memcpy(void* dest, void * src, int size){
    if(!dest || !src || size < 1) return ;

    uint8_t * s  = (uint8_t)src;
    uint8_t * d  = (uint8_t)dest;

    while(size--){
        *d++ = *s++;
    }
}

// 内存按字节数量进行设置
void kernel_memset(void * dest,uint8_t v, int size){
    if(!dest || size < 1) return;

    uint8_t * d = (uint8_t) dest;

    while(size--){
        *d++ = v;
    }
}

int kernel_memcpy(void * d1, void *d2, int size){

}

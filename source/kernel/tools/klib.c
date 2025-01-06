#include "include/tools/klib.h"
#include "include\tools\log.h"
#include "comm/cpu_instr.h"

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

// 字节拷贝
int kernel_memcmp(void * d1, void *d2, int size){
    if(!d1 || !d2 || size < 1) return -1;

    uint8_t * dp1 = (uint8_t)d1;
    uint8_t * dp2 = (uint8_t)d2;

    while(*dp1 == *dp2 && size--){
        dp1++;
        dp2++;
    }

    // 相等 个人定义：*s1 == *s2 完全一模一样才相等
    if(*dp1 == *dp2 && size == 0 ) return 0;

    if(*dp1 > *dp2) return 1;
    else{
        return -1;
    }
}

// 将 num 按照 指定进制base 写入 buf
void kernel_itoa(char * buf, int num, int base){
    char * p = buf;
    int old_num = num;
    if((base != 2) && (base != 8) && (base != 10) && (base != 16)){
        *P = '\0';
        return;
    }

    if((num < 0) && (base == 10)){
        *p++ = '-';
    }

    while (num > 0)
    {
        char ch = (char) (num % base + '0');
        *p++ =  ch;
        num /= base;
    }
    *p-- = '\0';
    
    // 区间反转
    char * start = buf;
    if(old_num < 0) 
        start++;
    while(start != p){
        char ch = *start;
        *start++ = *p;
        *p-- = ch;
    }

}

void kernel_sprint(char * buf, const char * fmt, ...){、
    va_list args;

    va_start(args, fmt);       
    kernel_vsprint(str_buf, fmt, args);
    va_end(args);
}

// "VERSION: %s", "1.0.1"

void kernel_vsprint(char * buf, const char * fmt, va_list args){
    // 定义状态机
    enum{NORMAL, READ_FMT} state = NORMAL;

    const char * curr = buf;

    char ch;

    while((ch = *fmt++)){
        switch (state)
        {
        case NORMAL:
            if(ch == '%'){
                state = READ_FMT;
            }else{
                 *curr++ = ch;
            }
            break;
        case READ_FMT:
            // 字符串类型
            if(ch == 's' ){
                // 通过 va_arg 取当前 可变参数列表中的 字符串成员
                const char * str = va_arg(args, char *);
                uint32_t len = (uint32_t)kernel_strlen(str);
                while(len--){
                    *curr++ = *str++;
                }
            } 

            else if(ch == 'd'){
                int num = va_arg(args, int);
                kernel_itoa(curr, num, 10);
                curr += kernel_strlen(curr);
            }
            else if(ch == 'x'){
                int num = va_arg(args, int);
                kernel_itoa(curr, num, 16);
                curr += kernel_strlen(curr);
            }else if(ch == 'c'){
                char c = va_arg(args, int);
                *curr++ = c;
            } 
            
            state = NORMAL;
        default:
            break;
        }
    }
}


void pannic(const char * file, int line, const char * func, const char * cond){
    log_print("assert failed ! %s", cond);
    log_print("file: %s\n" "line: %d\n" "func: %s\n", file, line, func);

    // 死机
    for (;;)
    {
       hlt();
    }
    
}
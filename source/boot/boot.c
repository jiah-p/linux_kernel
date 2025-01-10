__asm__(".code16gcc");

#include "boot.h"

#define	LOADER_START_ADDR  0x8000

/**
 * Boot的C入口函数
 * 只完成一项功能，即从磁盘找到loader文件然后加载到内容中，并跳转过去
 */
void boot_entry(void) {
    // 跳转到 loader 中执行        LOADER_START_ADDR 强转成函数 然后执行  
    ((void (*)(void))LOADER_START_ADDR)();
} 


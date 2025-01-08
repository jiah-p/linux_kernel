#ifndef __MEMORY_H__
#define __MEMORY_H__

// 注意本项目 使用的是 平坦模式：设定的内存模型，都是从 0地址开始进行偏移，所以 逻辑地址 等于 线性地址 （较为简单的模型，少了一层映射）

#include "comm/types.h"
#include "include/tools/bitmap.h"
#include "include/ipc/mutex.h"
#include "comm/boot_info.h"
#include "include/tools/klib.h"

#define MEM_EXT_START              (1024 * 1024)               // 1MB
#define MEM_PAGE_SIZE              4096                        // 4KB                 
#define MEM_EBDA_START             0x80000

typedef struct _addr_alloc_t{
    bitmap_t bitmap;

    uint32_t start;                     // 程序的起始地址
    uint32_t size;                      // 程序占用空间的大小
    uint32_t page_size;                 // 单个页的大小

    mutex_t mutex;
}addr_alloc_t;

void memory_init(boot_info_t * boot_info);

#endif
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "comm/types.h"
#include "include/tools/bitmap.h"
#include "include/ipc/mutex.h"
#include "comm/boot_info.h"

typedef struct _addr_alloc_t{
    bitmap_t bitmap;

    uint32_t start;                     // 程序的起始地址
    uint32_t size;                      // 程序占用空间的大小
    uint32_t page_size;                 // 单个页的大小

    mutex_t mutex;
}addr_alloc_t;

void memory_init(boot_info_t * boot_info);

#endif
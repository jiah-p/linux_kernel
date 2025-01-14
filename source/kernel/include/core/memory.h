#ifndef __MEMORY_H__
#define __MEMORY_H__

// 注意本项目 使用的是 平坦模式：设定的内存模型，都是从 0地址开始进行偏移，所以 逻辑地址 等于 线性地址 （较为简单的模型，少了一层映射）
// 一个进程 对应 一个 内存转换表
// 拟采用方案：page_size = 4MB 大小 进行 内存管理

#include "comm/types.h"
#include "tools/bitmap.h"
#include "ipc/mutex.h"
#include "comm/boot_info.h"
#include "tools/klib.h"
#include "cpu/mmu.h"


#define MEM_EXT_START              (1024 * 1024)               // 1MB
#define MEM_PAGE_SIZE              4096                        // 4KB                 
#define MEM_EBDA_START             0x80000
#define MEMORY_TASK_BASE           0x80000000
#define MEM_EXT_END                (127 * 1024 * 1024)         // 操作内存空间的末尾

typedef struct _addr_alloc_t{
    bitmap_t bitmap;

    uint32_t start;                     // 程序的起始地址
    uint32_t size;                      // 程序占用空间的大小
    uint32_t page_size;                 // 单个页的大小

    mutex_t mutex;
}addr_alloc_t;


// 描述存储区域
typedef  struct _memory_map_t
{
    //  虚拟地址
    void * vstart;
    void * vend;
    // 物理起始地址
    void * pstart;
    // 内存特权
    uint32_t perm;
}memory_map_t;

void memory_init(boot_info_t * boot_info);
int memory_create_map(pte_t * page_dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm);

uint32_t memory_create_uvm(void );

int memory_alloc_page_for(uint32_t addr, uint32_t size, uint32_t perm);

uint32_t memory_alloc_page(void);
void memory_free_page(uint32_t);

#endif
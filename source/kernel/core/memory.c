#include "include/core/memory.h"
#include "include/tools/log.h"

static void addr_alloc_init(addr_alloc_t * alloc, uint8_t * bits, uint32_t start, uint32_t size, uint32_t page_size){
    mutex_init(&alloc->mutex);
    alloc->start = start;
    alloc->size = size;
    alloc->page_size = page_size;
    
    bitmap_init(&alloc->bitmap, bits, alloc->size / page_size, 0); 
}

static uint32_t addr_alloc_page(addr_alloc_t * alloc, int page_count){
    uint32_t addr = 0;
    mutex_lock(&alloc->mutex);

    int page_index = bitmap_alloc_nbits(&alloc->bitmap, 0, page_count);

    if(page_index == -1){                           // 分配失败
        addr = alloc->start + page_index * alloc->page_size;            // 进程的起始地址 + 页索引 * 页大小
    }
    mutex_unlock(&alloc->mutex);

    return addr;
}

static uint32_t addr_free_page(addr_alloc_t * alloc, uint32_t addr, int page_count){
    mutex_lock(&alloc->mutex);

    uint32_t pg_index = (addr - alloc->start) / alloc->page_size;               // 程序中的偏移量 / 页大小
    bitmap_set_bit(&alloc->bitmap, pg_index, page_count, 0);

    mutex_unlock(&alloc->mutex);
}


void show_mem_info(boot_info_t * boot_info){
    log_print("mem region:");
    for (int i = 0; i < boot_info->ram_region_count; i++)
    {
        log_print("[%d]:0x%x - 0x%x", i, boot_info->ram_region_cfg[i].start, boot_info->ram_region_cfg[i].size);
    }
    
    log_print("");
}

static uint32_t total_mem_size(boot_info_t * boot_info){
    uint32_t mem_size = 0;

    for (int i = 0; i < boot_info->ram_region_count; i++)
    {
        mem_size += boot_info->ram_region_cfg[i].size;
    }
    
    return mem_size;
}

// 地址分配结构
static addr_alloc_t paddr_alloc;

void memory_init(boot_info_t * boot_info){
    // test
    // addr_alloc_t addr_alloc;

    // uint8_t bits[8];            // 8 * 8 * page_size

    // // 程序地址分配管理结构    缓冲区地址   起始地址：4kB  大小：64个4KB  页大小： 4KB
    // addr_alloc_init(&addr_alloc, bits, 0x1000, 64 * 4096, 4096);  

    // for (int i = 0; i < 32; i++)
    // {
    //     uint32_t addr = addr_alloc_page(&addr_alloc, 2);
    //     log_print("alloc addr: 0x%x", addr);
    // }

    // uint32_t free_addr = 0x1000;
    // for(int i = 0; i < 32; i++){
    //     addr_free_page(&addr_alloc, free_addr, 2);
    //     free_addr += 8192;
    // }            

    extern uint8_t * mem_fee_start;                 // kernel.lds 文件中提供 .bss后内存地址
    log_print("mem init");

    show_mem_info(boot_info);

    // 目前设计：1M 以上用于 进程分配与管理，1M以内用于 kernel，位图等存放 makefile
    uint32_t mem_above1MB_free = total_mem_size(boot_info) - MEM_EXT_START;
    mem_above1MB_free = down2(mem_above1MB_free, MEM_PAGE_SIZE)

    log_print("free memmory: 0x%x, size: 0x%x", MEM_EXT_START, mem_above1MB_free);

    uint8_t * mem_free = (uint8_t *)&mem_fee_start;

    // 地址分配     kernel[.text .rodata .data .bss] 位置 + 偏移
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_above1MB_free, MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);

    // 0x80000 是显示等内存保留区域
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);
}
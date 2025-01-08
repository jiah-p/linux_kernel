#include "core/memory.h"
#include "tools/log.h"
#include "cpu/mmu.h"

// 地址分配结构
static addr_alloc_t paddr_alloc;
static pde_t kernel_page_dir[PDE_CNT] __attribute__((aligned(MEM_PAGE_SIZE)));

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

pte_t * find_pte(pte_t * page_dir, uint32_t vaddr, int alloc){
    pde_t * page_table;
    pde_t * pde = page_dir + pde_index(vaddr);

    if(pde->present){
        page_table = (pte_t *)pde_paddr(pde);
    }else{
        if(alloc == 0){
            return (pte_t *)0;
        }

        uint32_t pg_addr = addr_alloc_page(&paddr_alloc, 1);
        if(pg_addr == 0){
            return (pte_t *)0;
        }

        pde->v = pg_addr | PDE_P;

        page_table = (pte_t *)pg_addr;
        kernel_memset(page_table, 0, MEM_PAGE_SIZE);
    }

    return page_table + pte_index(vaddr);
}


// 建立页表的地址映射关系 逻辑（虚拟） -> 物理
int memory_create_map(pde_t * page_dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm){
    for (int i = 0; i < count; i++)
    {
        pte_t * pte = find_pte(page_dir, vaddr, 1);                     // 1 如果未找到，则进行 PTE表的分配
        if(pte == (pte_t *)0){
            return -1;
        }

        // present 为 0 需要建立表项
        ASSERT(pte->present == 0);
        pte->v = paddr | perm | PTE_P;

        vaddr += MEM_PAGE_SIZE;
        paddr += MEM_PAGE_SIZE;
    }
    
}

void create_kernel_table(){
    extern uint8_t s_text[], e_text[], s_data[], kernel_base[];

    // 各区间内存段设计 OS
    static memory_map_t kernel_map[] = {
        {kernel_base, s_text, kernel_base,  PTE_W},
        // .text rodata 只读 .data 和 .bss mem_free_start 可读写
        {s_text, e_text, s_text, 0},
        {s_data, (void *)MEM_EBDA_START, s_data, PTE_W},
        
    };

    for (int i = 0; i < sizeof(kernel_map) / sizeof(memory_map_t); i++)
    {
        memory_map_t * map = kernel_map + i;

        
        uint32_t vstart = down2((uint32_t)map->vstart, MEM_PAGE_SIZE);          // 边界对齐
        uint32_t vend = up2((uint32_t)map->vend, MEM_PAGE_SIZE);
        uint32_t paddr = down2((uint32_t)map->pstart, MEM_PAGE_SIZE);
        uint32_t page_count = (vend - vstart) / MEM_PAGE_SIZE;

        // 建立映射关系
        memory_create_map(kernel_page_dir, vstart, (uint32_t)map->pstart, page_count, map->perm);
    }
    
}




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

    // show_mem_info(boot_info);

    // 目前设计：1M 以上用于 进程分配与管理，1M以内用于 kernel，位图等存放 makefile
    uint32_t mem_above1MB_free = total_mem_size(boot_info) - MEM_EXT_START;
    mem_above1MB_free = down2(mem_above1MB_free, MEM_PAGE_SIZE);

    log_print("free memmory: 0x%x, size: 0x%x", MEM_EXT_START, mem_above1MB_free);

    uint8_t * mem_free = (uint8_t *)&mem_fee_start;

    // 地址分配     kernel[.text .rodata .data .bss] 位置 + 偏移
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_above1MB_free, MEM_PAGE_SIZE);
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);

    // 0x80000 是显示等内存保留区域
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);

    create_kernel_table();
    mmu_set_page_dir((uint32_t)kernel_page_dir);
}
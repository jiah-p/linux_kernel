#ifndef __MMU_H__
#define __MMU_H__

#include "comm/types.h"
#include "comm/cpu_instr.h"

// 页表项数量
#define PDE_CNT                 1024
#define PTE_CNT                 1024
#define PTE_P                   (1 << 0)
#define PDE_P                   (1 << 0)


// perm
#define PDE_W                   (1 << 1)
#define PTE_W                   (1 << 1)
#define PDE_U                   (1 << 2)
#define PTE_U                   (1 << 2)

// 二级页表
typedef union _pde_t
{
    uint32_t v;
    struct 
    {
        uint32_t present        : 1;
        uint32_t write_enable   : 1;
        uint32_t user_mode_acc  : 1;
        uint32_t write_through  : 1;
        uint32_t cache_disable  : 1;
        uint32_t accessed       : 1;
        uint32_t                : 1;
        uint32_t pr             : 1;
        uint32_t                : 4;
        uint32_t phy_pt_addr    : 20;
    };
    
}pde_t;

// 一级页表
typedef union _pte_t
{
    uint32_t v;
    struct 
    {
        uint32_t present        : 1;
        uint32_t write_enable   : 1;
        uint32_t user_mode_acc  : 1;
        uint32_t write_through  : 1;
        uint32_t cache_disable  : 1;
        uint32_t accessed       : 1;
        uint32_t dirty          : 1;
        uint32_t pat            : 1;
        uint32_t global         : 1;
        uint32_t                : 3;
        uint32_t phy_page_addr  : 20;
    };
    
}pte_t;


static inline void mmu_set_page_dir(uint32_t paddr){
    write_cr3(paddr);
}

static inline uint32_t pde_index(uint32_t vaddr){
    int index = (vaddr >> 22);

    return index;
}

static inline uint32_t pte_index(uint32_t vaddr){
    return (vaddr >> 12) & 0x3ff;
}

static inline uint32_t pde_paddr(pde_t * pde){
    return pde->phy_pt_addr << 12;
}

static inline uint32_t pte_paddr(pte_t * pte){
    return pte->phy_page_addr << 12;
}

// 获取 pte 部分属性
static inline uint32_t get_pte_perm(pte_t * pte){
    return (pte->v & 0x3ff);
}

#endif
#ifndef __CPU_H__
#define __CPU_H__

#include "comm/types.h"

#define EFLAGS_DEFAULT              (1 << 1)
#define EFLAGS_IF                   (1 << 9)
#define True                        1

#pragma pack(1)     // 禁止填充
typedef struct _segment_desc_t 
{      
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
}segment_desc_t;


typedef struct _gate_desc_t
{
    uint16_t offset15_0;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset31_16;
}gate_desc_t;

// 32-Bit Task_State Segment(TSS)   7.2.1 卷3  
typedef struct _tss_t   
{
    uint32_t pre_link;
    uint32_t eps0, ss0, esp1, ss1, esp2, ss2;   // 保护模式下 不同特权级栈描述符
    // ss0 ss1 ss2 用于 特权级 变化 从而引发 栈空间变化的栈段
    uint32_t cr3;                               // 页表
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;            // ss 为 ss3
    uint32_t ldt;                               // ldt 表
    uint32_t iomap;                             // 接口访问位图
}tss_t;


#define GATE_P_PRESENT       (1 << 15)
#define GATE_DPL0            (0 << 13)
#define GATE_DPL3            (3 << 13)  // 0b11 << 13
#define GATE_TYPE_INT        (0xE << 8)
#define GATE_TYPE_SYSCALL    (0xC << 8)

#pragma pack()

#define SEG_G           (1 << 15) 
#define SEG_D           (1 << 14)
#define SEG_P_PRESENT   (1 << 7)

#define SEG_DPL0        (0 << 5)
#define SEG_DPL3        (3 << 5)

#define SEG_CPL0        (0 << 0)
#define SEG_CPL3        (3 << 0)

#define SEG_S_SYSTEM    (0 << 4)
#define SEG_S_NORMAL    (1 << 4)

#define SEG_TYPE_CODE   (1 << 3)
#define SEG_TYPE_DATA   (0 << 3)
#define SEG_TYPE_TSS    (9 << 0)

#define SEG_TYPE_RW     (1 << 1)

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);

void gate_desc_set(gate_desc_t * desc, uint16_t selector, uint32_t offset, uint16_t attr);

void init_gdt(void);
 
void cpu_init(void);

void init_idt(void);

int gdt_alloc_desc(void);

void switch_to_tss(int);

void gdt_free_sel(int);

#endif
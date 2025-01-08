#ifndef CPU_INSTR_h
#define CPU_INSTR_h
// 一般放头文件的函数，做内联，用 static 避免重复文件生

#include "types.h"



// 关中断
static inline void cli(void){
    __asm__ __volatile("cli");
}

// 开中断
static inline void sti(void){
    __asm__ __volatile("sti");
}

static inline uint8_t inb(uint16_t port){
    uint8_t rv;
    __asm__ __volatile(
        "inb %[p], %[v]"
        :[v]"=a"(rv)
        :[p]"d"(port)
    );     // inb port rv

    return rv;
}

static inline uint8_t inw(uint16_t port){
    uint16_t rv;
    __asm__ __volatile(
        "in %[p], %[v]"
        :[v]"=a"(rv)
        :[p]"d"(port)
    );     // inb port rv

    return rv;
}


static inline uint8_t outb(uint16_t port, uint8_t data){
    // outb al,dx
    __asm__ __volatile(
        "outb %[v], %[p]"
        ::[p]"d"(port),[v]"a"(data)
    ); 
}

static inline void lgdt(uint32_t start, uint32_t size){
    struct 
    {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    }gdt;
    
    gdt.start31_16 = start >> 16;
    gdt.start15_0 = start & 0xFFFF;
    gdt.limit = size - 1;

    __asm__ __volatile(
    "lgdt %[g]"::[g]"m"(gdt)
    );
}

static inline uint32_t read_cr0(){
    uint32_t cr0;

    __asm__ __volatile(
        "mov %cr0, %[v]"
        :[v]"=r"(cr0)
    ); 

    return cr0;
}

static inline void write_cr0(uint32_t v){
    __asm__ __volatile(
        "mov %[v], %cr0"
        ::[v]"r"(v)
    ); 
}


static inline void far_jump(uint32_t selector, uint32_t offset){
    uint32_t addr[] = {offset, selector};

    __asm__ __volatile__(
        "ljmpl *(%[a])"::[a]"r"(addr)
    );
}

// 封装 汇编代码 初始化 idt表的加载
static inline void lidt(uint32_t start, uint32_t size){
    struct 
    {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    }idt;
    
    idt.start31_16 = start >> 16;
    idt.start15_0 = start & 0xFFFF;
    idt.limit = size - 1;

    __asm__ __volatile(
    "lidt %[g]"::[g]"m"(idt)
    );
}

static void hlt(void){
    __asm__ __volatile("hlt");
}

static inline void write_tr(uint16_t tss_sel){
    __asm__ __volatile__("ltr %%ax"::"a"(tss_sel));
}


static inline uint32_t read_eflags(void){
    uint32_t eflags;

    __asm__ __volatile__("pushf\n\tpop %%eax":"=a"(eflags));

    return eflags;
}

// 写入 中断状态 eflags 寄存器
static inline void write_eflags(uint32_t eflags){
    __asm__ __volatile__("push %%eax\n\tpop"::"a"(eflags));
}

#endif
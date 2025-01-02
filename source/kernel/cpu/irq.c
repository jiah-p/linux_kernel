#include "include/cpu/irq.h"
#include "include/cpu/cpu.h"
#include "include/os_cfg.h"

#define IDE_TABLE_NR         128

void exception_handler_unknown(void);

static gate_desc_t idt_table[IDE_TABLE_NR];

void irq_init(void){
    for(int i = 0; i < IDE_TABLE_NR; i++){
        gate_desc_set(idt_table + i, KEREL_SELECTOR_CS, (uint32_t)exception_handler_unknown, 
        GETE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT );
    }

    irq_install(IRQ0_DE, (irq_handler_t)exception_handler_divider);

    lidt((uint32_t)idt_table, sizeof(idt_table))
}

// 这个结构体 作为参数去找栈上面部分的元素 的方法，然后注意 汇编层面 的实现
static void do_default_handler(exception_frame_t * frame, const char * msg){
    for(;;){}   // 模拟 死机 
}

// 异常捕获
void do_handler_unknown(exception_frame_t * frame){
    do_default_handler(frame, "unknown exception");     // 默认处理程序
}

void exception_handler_divider(exception_frame_t * frame){
    do_default_handler(frame, "Divider exception");
}

int irq_install(int irq_num, irq_handler_t handler){
    if(irq_Num >= IDE_TABLE_NR){
        return -1;
    }

    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, 
    (uint32_t)handler, GETE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);

    return 0;
}
#include "include/cpu/irq.h"
#include "include/cpu/cpu.h"
#include "include/os_cfg.h"

#define IDE_TABLE_NR         128

void exception_handler_unknown(void);

static gate_desc_t idt_table[IDE_TABLE_NR];

// 初始化 8259 芯片 ICW1-4
static void init_pic(void){
    // 第一块芯片 初始化
    outb(PIC0_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    outb(PIC0_ICW2, IRQ_PIC_START);
    outb(PIC0_ICW3, 1 << 2);
    outb(PIC0_ICW4, PIC_ICW4_8086);
    // 第二块芯片 初始化
    outb(PIC1_ICW1, PIC_ICW1_ALWAYS_1 | PIC_ICW1_ICW4);
    outb(PIC1_ICW2, IRQ_PIC_START);
    outb(PIC1_ICW3, 2);
    outb(PIC1_ICW4, PIC_ICW4_8086);

    // 中断屏蔽寄存器 初始化
    outb(PIC0_IMR, 0xFF & ~(1 << 2));   // 第一块芯片不屏蔽第二块芯片的信号
    outb(PIC1_IMR, 0xff)

}

void irq_init(void){
    for(int i = 0; i < IDE_TABLE_NR; i++){
        gate_desc_set(idt_table + i, KEREL_SELECTOR_CS, (uint32_t)exception_handler_unknown, 
        GETE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT );
    }

    irq_install(IRQ0_DE, (irq_handler_t)exception_handler_divider);
    irq_install(IRQ1_DB, (irq_handler_t)exception_handler_Debug);
    irq_install(IRQ2_NMI, (irq_handler_t)exception_handler_NMI);
    irq_install(IRQ3_BP, (irq_handler_t)exception_handler_breakpoint);
    irq_install(IRQ4_OF, (irq_handler_t)exception_handler_overflow);
    irq_install(IRQ5_BR, (irq_handler_t)exception_handler_bound_range);
    irq_install(IRQ6_UD, (irq_handler_t)exception_handler_invalid_opcode);
    irq_install(IRQ7_NM, (irq_handler_t)exception_handler_device_unavabliable);
    irq_install(IRQ8_DF, (irq_handler_t)exception_handler_double_fault);
    irq_install(IRQ10_TS, (irq_handler_t)exception_handler_invalid_tss);
    irq_install(IRQ11_NP, (irq_handler_t)exception_handler_sgement_not_present);
    irq_install(IRQ12_SS, (irq_handler_t)exception_handler_stack_segment_fault);
    irq_install(IRQ13_GP, (irq_handler_t)exception_handler_general_protection);
    irq_install(IRQ14_PF, (irq_handler_t)exception_handler_page_fault);
    irq_install(IRQ16_MF, (irq_handler_t)exception_handler_fpu_error);
    irq_install(IRQ17_AC, (irq_handler_t)exception_handler_aligment_check);
    irq_install(IRQ18_MC, (irq_handler_t)exception_handler_machine_check);
    irq_install(IRQ19_XM, (irq_handler_t)exception_handler_simd_exception);
    irq_install(IRQ20_VE, (irq_handler_t)exception_handler_virtual_exception);
    irq_install(IRQ21_CP, (irq_handler_t)exception_handler_control_exception);


    lidt((uint32_t)idt_table, sizeof(idt_table))

    init_pic();
}

// 这个结构体 作为参数去找栈上面部分的元素 的方法，然后注意 汇编层面 的实现
static void do_default_handler(exception_frame_t * frame, const char * msg){
    // 模拟 死机 
    for(;;){
        // 暂停运行 cpu
        hlt();
    }   

}

// 异常捕获
void do_handler_unknown(exception_frame_t * frame){
    do_default_handler(frame, "unknown exception");     // 默认处理程序
}

void exception_handler_divider(exception_frame_t * frame){
    do_default_handler(frame, "Divider exception");
}
void exception_handler_Debug(exception_frame_t * frame){
    do_default_handler(frame, "Debug exception");
}

void exception_handler_NMI(exception_frame_t * frame){
    do_default_handler(frame, "NMI exception");
}

void exception_handler_breakpoint(exception_frame_t * frame){
    do_default_handler(frame, "breakpoint exception");
}

void exception_handler_overflow(exception_frame_t * frame){
    do_default_handler(frame, "overflow exception");
}

void exception_handler_bound_range(exception_frame_t * frame){
    do_default_handler(frame, "bound_range exception");
}

void exception_handler_invalid_opcode(exception_frame_t * frame){
    do_default_handler(frame, "invalid_opcode exception");
}

void exception_handler_device_unavabliable(exception_frame_t * frame){
    do_default_handler(frame, "device_unavabliable exception");
}

void exception_handler_double_fault(exception_frame_t * frame){
    do_default_handler(frame, "double_fault exception");
}

void exception_handler_invalid_tss(exception_frame_t * frame){
    do_default_handler(frame, "invalid_tss exception");
}

void exception_handler_sgement_not_present(exception_frame_t * frame){
    do_default_handler(frame, "sgement_not_present exception");
}

void exception_handler_stack_segment_fault(exception_frame_t * frame){
    do_default_handler(frame, "stack_segment_fault exception");
}

void exception_handler_general_protection(exception_frame_t * frame){
    do_default_handler(frame, "general_protection exception");
}

void exception_handler_page_fault(exception_frame_t * frame){
    do_default_handler(frame, "page_fault exception");
}

void exception_handler_fpu_error(exception_frame_t * frame){
    do_default_handler(frame, "fpu_error exception");
}

void exception_handler_aligment_check(exception_frame_t * frame){
    do_default_handler(frame, "aligment_check exception");
}

void exception_handler_machine_check(exception_frame_t * frame){
    do_default_handler(frame, "machine_check exception");
}

void exception_handler_simd_exception(exception_frame_t * frame){
    do_default_handler(frame, "simd_exception exception");
}

void exception_handler_virtual_exception(exception_frame_t * frame){
    do_default_handler(frame, "virtual_exception exception");
}

void exception_handler_control_exception(exception_frame_t * frame){
    do_default_handler(frame, "control_exception exception");
}


int irq_install(int irq_num, irq_handler_t handler){
    if(irq_Num >= IDE_TABLE_NR){
        return -1;
    }

    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, 
    (uint32_t)handler, GETE_P_PRESENT | GATE_DPL0 | GATE_TYPE_INT);

    return 0;
}

// 开中断
void irq_enable(int irq_num){
    if(irq_num < IRQ_PIC_START){
        return;
    }
    
    irq_num -= IRQ_PIC_START;       //  调整中断序号：0 - 15 
    if(irq_num < 8){
        uint8_t mask = inb(PIC0_IMR) & ~(1 << irq_num);     // 屏蔽掩码 
        outb(PIC0_IMR, mask);   // 回写 
    }else{ 
        irq_num -= 8;
        uint8_t mask = inb(PIC1_IMR) & ~(1 << irq_num);
        outb(PIC1_IMR, mask);
    }
}

// 关中断
void irq_disable(int irq_num){
    if(irq_num < IRQ_PIC_START){
        return;
    }
    
    irq_num -= IRQ_PIC_START;
    if(irq_num < 8){
        uint8_t mask = inb(PIC0_IMR) | (1 << irq_num);     // 屏蔽掩码 
        outb(PIC0_IMR, mask);   // 回写 
    }else{
        irq_num -= 8;      
        uint8_t mask = inb(PIC1_IMR) | (1 << irq_num);
        outb(PIC1_IMR, mask); 
    }
}

void irq_disable_global(void){
    cli();
}

void irq_enable_global(void){
    sti();
}

void pic_send_eoi(int irq_num){
    irq_num -= IRQ_PIC_START;
    
    if(irq_num >= 8){
        outb(PIC1_OCW2, PIC_OCW2_EOI);
    }else{
        outb(PIC0_OCW2, PIC_OCW2_EOI);
    }
    
}
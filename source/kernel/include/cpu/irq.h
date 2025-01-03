#ifndef __IRQ_H__
#define __IRQ_H__

// X86芯片开发 第二章 6.15 部分介绍了 该中断对应的知识
#define IRQ0_DE         0
#define IRQ1_DB         1
#define IRQ2_NMI        2
#define IRQ3_BP         3
#define IRQ4_OF         4
#define IRQ5_BR         5
#define IRQ6_UD         6
#define IRQ7_NM         7
#define IRQ8_DF         8
#define IRQ10_TS        10
#define IRQ11_NP        11
#define IRQ12_SS        12
#define IRQ13_GP        13
#define IRQ14_PF        14
#define IRQ16_MF        16
#define IRQ17_AC        17
#define IRQ18_MC        18
#define IRQ19_XM        19
#define IRQ20_VE        20
#define IRQ21_CP        21


typedef struct  _exception_frame_t
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, error_code;       // 序号 和 错误码
    uint32_t eip, cs, eflags;

}exception_frame_t;

typedef void (*irq_handler_t)(exception_frame_t * frame);

void irq_init(void);

// 异常处理函数：按照芯片第三卷手册进行定义雨开发
void exception_handler_divider(void);
void exception_handler_Debug(void);
void exception_handler_NMI(void);
void exception_handler_breakpoint(void);
void exception_handler_overflow(void);
void exception_handler_bound_range(void);
void exception_handler_invalid_opcode(void);
void exception_handler_device_unavabliable(void);
void exception_handler_double_fault(void);
void exception_handler_invalid_tss(void);
void exception_handler_sgement_not_present(void);
void exception_handler_stack_segment_fault(void);
void exception_handler_general_protection(void);
void exception_handler_page_fault(void);
void exception_handler_fpu_error(void);   // float processor unit
void exception_handler_aligment_check(void);
void exception_handler_machine_check(void);
void exception_handler_simd_exception(void);
void exception_handler_virtual_exception(void);
void exception_handler_control_exception(void);


int irq_install(int irq_Num, irq_handler_t handler);

#endif
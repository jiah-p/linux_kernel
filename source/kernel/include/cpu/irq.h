#ifndef __IRQ_H__
#define __IRQ_H__

#include "comm/types.h"

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


#define IRQ0_TIMER      0x20

// PIC芯片 中断寄存器
#define PIC0_ICW1        0x20
#define PIC0_ICW2        0x21
#define PIC0_ICW3        0x21
#define PIC0_ICW4        0x21
#define PIC0_IMR         0x21
#define PIC0_OCW2        0x20
// 第二块
#define PIC1_ICW1        0xA0
#define PIC1_ICW2        0xA1
#define PIC1_ICW3        0xA1
#define PIC1_ICW4        0xA1
#define PIC1_IMR         0xA1
#define PIC1_OCW2        0xA0
 

#define PIC_ICW1_ALWAYS_1       (1 << 4)
#define PIC_ICW1_ICW4           (1 << 0)
#define PIC_ICW4_8086           (1 << 0)

#define PIC_OCW2_EOI            (1 << 5)

#define IRQ_PIC_START           0x20

#define ERR_PAGE_P              (1 << 0)
#define ERR_PAGE_WR             (1 << 1)
#define ERR_PAGE_US             (1 << 2)

// GP fault
#define ERR_EXT                 (1 << 0)
#define ERR_IDT                 (1 << 1)

typedef struct  _exception_frame_t
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, error_code;       // 序号 和 错误码
    uint32_t eip, cs, eflags;
    uint32_t esp3, ss3;
}exception_frame_t;

typedef void (*irq_handler_t)(exception_frame_t * frame);

void irq_init(void);

// 异常处理函数：按照芯片第三卷手册进行定义雨开发
void exception_handler_divider(exception_frame_t * frame);
void exception_handler_Debug(exception_frame_t * frame);
void exception_handler_NMI(exception_frame_t * frame);
void exception_handler_breakpoint(exception_frame_t * frame);
void exception_handler_overflow(exception_frame_t * frame);
void exception_handler_bound_range(exception_frame_t * frame);
void exception_handler_invalid_opcode(exception_frame_t * frame);
void exception_handler_device_unavabliable(exception_frame_t * frame);
void exception_handler_double_fault(exception_frame_t * frame);
void exception_handler_invalid_tss(exception_frame_t * frame);
void exception_handler_sgement_not_present(exception_frame_t * frame);
void exception_handler_stack_segment_fault(exception_frame_t * frame);
void exception_handler_general_protection(exception_frame_t * frame);
void exception_handler_page_fault(exception_frame_t * frame);
void exception_handler_fpu_error(exception_frame_t * frame);   // float processor unit
void exception_handler_aligment_check(exception_frame_t * frame);
void exception_handler_machine_check(exception_frame_t * frame);
void exception_handler_simd_exception(exception_frame_t * frame);
void exception_handler_virtual_exception(exception_frame_t * frame);
void exception_handler_control_exception(exception_frame_t * frame);
void exception_handler_time(void);


int irq_install(int irq_Num, irq_handler_t handler);

void irq_enable(int irq_num);
void irq_disable(int irq_num);
void irq_disable_global(void);
void irq_enable_global(void);

void pic_send_eoi(int irq_num);

typedef uint32_t irq_state_t;

irq_state_t irq_enter_protection(void);
void irq_leave_protection(irq_state_t state);

#endif
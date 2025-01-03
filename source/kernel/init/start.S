#include  "include/os_cfg.h"
    .text
    .extern kernel_init
    .global _start 
    .extern init_main 
    # void start (boot_info_t * boot_info)
_start:
    ; push %ebp
    ; mov %esp, %ebp
    ; mov 0x8(%enp), %eax

    mov 4(%esp), %eax
    # kernel_init(boot_info)
    push %eax
    call kernel_init

    jmp $KERNEL_SELECTOR_CS, $gdt_reload

gdt_reload:

    mov KERNEL_SELECTOR_CS, %ax  
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %js
	mov %ax, %gs

    mov $(stack + KERNEL_STACK_SIZE), %esp
    jmp init_main
    
    .bss

.comm stack, KERNEL_STACK_SIZE

; 汇编代码 宏替换
; 硬件可能会自动压入 error_code 错误码
.macro exception_handler name num with_error_code
    .text
    .extern do_handler_\name
    .global exception_handler_\name

    # C 语言反汇编的这个中断返回指令是 ret 而不是 iret 所以必须使用
    # 汇编代码进行实现
exception_handler_\name:
    .if \with_error_code == 0       ; 错误码
        push $0
    .endif 

    push $\num # 压入序号
    # 保存现场
    # 保存 芯片自动保存以外的其他寄存器
    pusha   # 会自动保存 一些通用寄存器比如 eax ebx ... esp
    push %ds
    push %es
    push %fs
    push %gs 

    # 将 esp 压入栈中 此时栈顶保存了 frame 指针的地址
    push %esp

    call do_handler_\name

    ; pop %esp
    add $(1*4), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    ; 弹出 错误号 和 异常号
    add $(2*4), %esp 

    iret 
.endm

;  默认的缺省异常 : 名称 错误号 异常号
exception_handler unknown, -1, 0

;  除 0 异常处理函数 
exception_handler divider, 0, 0


exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavabliable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler sgement_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0      // float processor unit
exception_handler aligment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler simd_exception, 19, 0
exception_handler virtual_exception, 20, 0
exception_handler control_exception, 21, 1





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


    .text
    .extern do_handler_unknown
    .global exception_handler_unknown

    # C 语言反汇编的这个中断返回指令是 ret 而不是 iret 所以必须使用
    # 汇编代码进行实现
exception_handler_unknown:
    call do_handler_unknown
    iret 

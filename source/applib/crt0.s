
    .text
    .global _start
    .extern cstart
_start:
    mov %ss, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %gs

    jmp cstart
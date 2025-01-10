    .text
	.global	first_task_entry
	.extern first_task_main
first_task_entry:
	mov %ss, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %js
	mov %ax, %gs
    ; 跳转到 C文件 入口函数
	jmp  first_task_main       
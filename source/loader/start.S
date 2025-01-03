
  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern loader_entry   // 声明外部文件

_start:
	jmp loader_entry

	// 32位代码
	.code32
	.text
	.global	protect_mode_entry
	.extern loader_kernel
protect_mode_entry:
	mov $16,  %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %js
	mov %ax, %gs

	jmp $8, loader_kernel






	#include "boot.h"

  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry   // 声明外部文件
// 0x7c00 位置进行运行
// gcc 支持的 汇编语法 
// 指令op 源数据 目的数据
_start:
	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov $_start, %esp  // 等价于 mov $0x7c00, %esp

	// 准备环境 调用 INT10 进行字符显示 调用 BIOS 中断服务程序
	mov $0xe, %ah
	mov %'L', %al
	int $0x10

read_loader:
	mov $0x8000, %bx
	mov $0x2, %ah
	mov $0x2, %cx  	 	// cx {ch: cl} 所以 0x2 等于 ch = 0 cl = 2 第一个扇区坐标是1 ，所以loader是2
	mov $64, %al		// 一个扇区是 512B，512B*32 = 64KB
	mov $0x0080, %dx  	// dx {dh: dl}
	int $0x13		

	// 判断出口参数 cf
	// jc 会判断 c标志是否为 1，是则跳转到 label read_loader
	jc read_loader


	jmp boot_entry

	jmp . 
	
	// 引导结束段
	.section boot_end, "ax" 

boot_sig: .byte 0x55, 0xaa

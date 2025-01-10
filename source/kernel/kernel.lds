SECTIONS{
    PROVIDE(kernel_base = .);
    . = 0x10000;

    PROVIDE(s_text = .);
    . text : {
        *(EXCLUDE_FILE(*(first_task*)) .text)
    }
    . rodata : {
        *(EXCLUDE_FILE(*(first_task*)) .rodata)
    }
    PROVIDE(e_text = .);

    . = ALIGN(4096);
    PROVIDE(s_data = .);
    . data : {
        *(EXCLUDE_FILE(*(first_task*)) .data)
    }
    
    /* .data 0x1000 AT(0x0){

    } */

    . bss : {
        *(EXCLUDE_FILE(*(first_task*)) .bss)
    }   
    e_data = .;

    . = 0x80000000;
    PROVIDE(s_first_task = LOADADDR(.first_task));
    .first_task : AT(e_data) {
        *first_task_entry*(.text .rodata .bss .data)
        *first_task*(.text .rodata .bss .data)
    }
    PROVIDE(e_first_task = LOADADDR(.first_task) + SIZEOF(.first_task));

    /* 取出 bss后面位置的内存的地址  PROVIDE gcc提供，支持C文件使用*/
    PROVIDE(mem_free_start = e_first_task);
}
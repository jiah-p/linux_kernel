SECTIONS{
    PROVIDE(kernel_base = .);
    . = 0x10000;

    PROVIDE(s_text = .);
    . text : {
        *(.text)
    }
    . rodata : {
        *(.rodata)
    }
    PROVIDE(e_text = .);

    . = ALIGN(4096);
    PROVIDE(s_data = .);
    . data : {
        *(.data)
    }

    . bss : {
        *(.bss)
    }

    /* 取出 bss后面位置的内存的地址  PROVIDE gcc提供，支持C文件使用*/
    PROVIDE(mem_free_start = .);
}
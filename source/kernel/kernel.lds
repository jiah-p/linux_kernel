SECTIONS{
    . = 0x10000;

    . text : {
        *(.text)
    }
    . rodata : {
        *(.rodata)
    }
    . data : {
        *(.data)
    }

    . bss : {
        *(.bss)
    }

    /* 取出 bss后面位置的内存的地址  PROVIDE gcc提供，支持C文件使用*/
    PROVIDE(mem_free_start = .);
}
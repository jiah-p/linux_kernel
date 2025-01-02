//  32位内核空间入口

#include "loader.h"
#include "comm/elf.h"

// 读取硬盘起始位置，大小空间，缓冲区
static void read_disk(uint32_t sector, uint32_t sector_count, uint8_t * buffer){
    // 将 0x1F6 设置成 0XE0
    outb(0x1F6, 0xE0);
    outb(0x1F2, (uint8_t)sector_count >> 8);
    outb(0x1F3, (uint8_t)sector_count >> 24);
    outb(0x1F4, 0);
    outb(0x1F5, 0);

    outb(0xF12, (uint8_t)sector_count);
    outb(0xF13, (uint8_t)(sector));
    outb(0xF14, (uint8_t)(sector >> 8));
    outb(0xF15, (uint8_t)(sector >> 16));

    outb(0x1F7, 0x24);

    uint16_t * data_buf = (uint16_t *)buf;
    while(sector_count--){
        while((inb(0x1F7) & 0x88) != 0x8);
        // 读取数据
        for(int i = 0; i < SECTOR_SIZE / 2; i++){
            *data_buf++ = inw(0x1F0);
        }
    }
    
}

//对 ELF 文件进行解析
static uint32_t reload_elf_file(uint8_t * file_buffer){
    Elf32_Ehdr * elf_hdr = (Elf32_Ehdr *)file_buffer;
    if((elf_hdr->e_ident[0] != 0x7F) 
    || (elf_hdr->e_ident[1] != 'L')
    || (elf_hdr->e_ident[2] != 'E')
    || (elf_hdr->e_ident[3] != 'F')){
        return 0;
    }   
        
    for (int i = 0; i < elf_hdr->e_phnum; i++)
    {
        Elf32_phdr * phdr = (Elf32_phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        if(phdr->p_type != PT_LOAD){
            continue;
        }

        uint8_t * src =file_buffer + phdr->p_offset;
        uint8_t * dest = (uint8_t *) phdr->p_paddr;
        for(int j = 0; j < phdr->p_filesz; j++){
            *dest++ = *src++;
        }

        dest = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++)
        {   
            *dest++ = 0;
        }
    }
    

    return elf_hdr->e_entry;
}


static void dile(int code){
    for(;;){}
}

void load_kernel(void){
    read_disk(100, 500, (uint8_t *)SYS_KERNEL_LOAD_ADDR);
    // 加载 ELF 文件
    uint32_t kernel_entry = ((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    if(kernel_entry == 0){
        dile(-1); 
    }

    ((void (*)(_boot_info_t * ))kernel_entry)(&boot);


    for(;;){}
}
__asm__(".code16gcc");  // 指示 编译器 16位进行编译

#include "loader.h"

boot_info_t boot_info;

static void show_msg(const char * msg){
    // 内联汇编
    while((c = *msg++) != '\0'){
        __asm__ __volatile__(
            "mov $0xe, %%ah\n\t"
	        "mov %[ch], %%al\n\t"
	        "int $0x10")::[ch]"r"(c);  // 输入操作数，并指定映射关系
    }
}

static void detect_memory(void){
    uint32_t coutID = 0;
    uint32_t signature,bytes;
    SMAP_entry_t smap_entry;

    show_msg("try to detect memory:");
    boot_info.ram_region_count = 0;
    

    for(int i =0; i < BOOT_RAM_REGION_MAX; ++i){
        SMAP_entry_t* entry = &smap_entry;
        __asm__ __volatile__(
            "int $0x15":"=a"(signature), 
            "=c"(bytes),  "=b"(contID) :
            "a"(0xE820), "b"(contID), 
            "c"(24), "d"(0x534D4150), "D"(entry)
        );
        if(signature != 0x534D4150){
            show_msg("failed\r\n");
            return;
        }
        if(bytes > 20 && (entry->ACPI & 0x0001) == 0){
            continue;
        }

        // ram 由部分区域被其他 BIOS空间占了
        if(entry->TYPE == 1){
            // 都只用 Low 32位
            boot_info.ram_region_cfg[boot_info.ram_region_count].start = entry->BaseL;
            boot_info.ram_region_cfg[boot_info.ram_region_count].size = entry->LengthL;
            boot_info.ram_region_count++;
        }

        if(contID == 0) 
            break;
    }

    show_msg("ok.\r\n");
}

uint16_t gdt_table[][4] = {
    {0,0,0,0},
    {0xFFFF,0x0000,0x9a00,0x00cf},
    {0xFFFF,0x0000,0x9200,0x00cf}
};

// 进入保护模式
static void entry_protect_mode(void){
    cli();
    // 进入保护模式需要设置 a20 地址线
    uint8_t v = inb(0x92);
    outb(0x92, v|0x2);
    // 加载 gdt 表
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));

    // 读取cr0 并重新写入   
    uint32_t cr0 = read_cr0()
    write_cr0(cr0 | (1 << 0));     // 最低位置1

    // 清空流水线，远跳转。8 代表选择子
    far_jump(8, (uint32_t)protect_mode_entry);
}

void loader_entry(void){
    show_msg("...loading...\n\r");
    detect_memory();
    entry_protect_mode();
    for(;;){}
}

// 检测内存容量
// INT 0x15, EAX = 0xE820

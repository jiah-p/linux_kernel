#ifndef __OS_CFG__
#define __OS_CFG__


#define GDT_TABLE_SIZE          256
#define KERNEL_SELECTOR_CS      (1 * 8)
#define KERNEL_SELECTOR_DS      (2 * 8)
#define KERNEL_STACK_SIZE       (8 * 1024)

#endif
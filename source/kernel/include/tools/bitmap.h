#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "comm/types.h"

typedef struct _bitmap_t{
    int bit_count;              // 位图中位的数量
    uint8_t * bits;
}bitmap_t;

void bitmap_init(bitmap_t * bitmap, uint8_t * bits, int count, int init_bit);               // init_bit 初始化位图

int bitmap_byte_count(int bit_count);

// 获取某个位（页）是否空闲
int bitmap_getbit(bitmap_t * bitmap, int index);

// 设置某个位（页）空闲/忙
void bitmap_set_bit(bitmap_t * bitmap, int index, int count, int bit);                      // 从 index 开始，连续设置 count 个位设置为 bit

// 扫描位图 进行 分配 返回 对应的位置
int bitmap_alloc_nbits(bitmap_t * bitmap, int bit, int count);

// 判断某个位（页）是否忙
int bitmap_is_set(bitmap_t * bitmap, int index);

#endif
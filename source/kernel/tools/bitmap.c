#include "include/tools/bitmap.h"
#include "include/tools/klib.h"

int bitmap_byte_count(int bit_count){
    return (bit_count + 8 - 1) / 8;             // 向上取整
}

void bitmap_init(bitmap_t * bitmap, uint8_t * bits, int count, int init_bit){
    bitmap->bit_count = count;
    bitmap->bits = bits;

    int bytes = bitmap_byte_count(bitmap->bit_count);
    kernel_memset(bitmap->bits, init_bit ? 0xFF : 0, bytes);
}

int bitmap_getbit(bitmap_t * bitmap, int index){
    // 23 = 16 （第一行，第二行） + 7 （从右往左边数第 7 个）
    return bitmap->bits[index / 8] & (1 << (index % 8));           
}


void bitmap_set_bit(bitmap_t * bitmap, int index, int count, int bit){
    for(int i = 0; (i < count) && (index < bitmap->bit_count); i++, index++){
        if(bit){
            bitmap->bits[index / 8] |= (1 << (index % 8));
        }else{
            bitmap->bits[index / 8] &= ~(1 << (index % 8));
        }
    }
}


int bitmap_alloc_nbits(bitmap_t * bitmap, int bit, int count){
    int search_index = 0;
    int ok_index = -1;

    while(search_index < bitmap->bit_count){
        if(bitmap->bits[search_index] != bit){
            search_index++;
            continue;
        }

        // 找到 bit 指定的位置
        ok_index = search_index;
        int i = 1;
        for (; (i < count) && (search_index < bitmap->bit_count); i++, search_index++)
        {
            // 当前位置不满足
            if(bitmap_getbit(bitmap, search_index++) != bit){
                ok_index = -1;
                break;
            }
        }

        if(i >= count){
            bitmap_set_bit(bitmap, ok_index, count, ~bit);
            return ok_index;
        }
    }

    return -1;
}


int bitmap_is_set(bitmap_t * bitmap, int index){
    return bitmap_getbit(bitmap, index) ? 1 : 0;
}
#ifndef __LIST_H__
#define __LIST_H__

#include "comm/types.h"

// 结点作为索引 ——成员去寻址结构体本身 通过 偏移 进行计算   -> oop 中可以用继承进行设计
typedef struct _list_node_t
{
    struct _list_node_t * pre;
    struct _list_node_t * next;
}list_node_t;

static inline list_node_init(list_node_t * node){
    node->next = node->pre = (list_node_t * ) 0;
}

static inline list_node_t * list_node_pre(list_node_t * node){
    return node->pre;
}

static inline list_node_t * list_node_next(list_node_t * node){
    return node->next;
}


typedef struct _list_t
{
    list_node_t * first;
    list_node_t * last;
    uint32_t count;
}list_t;

void list_init(list_t * list);

static inline int list_empty(list_t * list){
    return list->count == 0;
}

static inline uint32_t list_count(list_t * list){
    return list->count;
}

static inline list_node_t * list_first(list_t * list){
    return list->first;
}

static inline list_node_t * list_last(list_t * list){
    return list->last;
}

// 头插
void list_insert_first(list_t * list, list_node_t * node);

// 尾插
void list_insert_last(list_t * list, list_node_t * node);

// 删除节点
list_node_t * list_remove_first(list_t * list);

list_node_t * list_remove(list_t * list, list_node_t * node);

// 通过 宏定义 计算 结点反推的偏移
#define offset_in_parent(parent_type, node_name)    ((uint32_t)(((parent_type *)0&)->node_name))                    // 结点在结构体中的偏移量

#define parent_addr(node, parent_type, node_name)   ((uint32_t)node - offset_in_parent(parent_type, node_name))     // 结构体中的地址，栈：高地址 - 偏移量 = 低地址     

#define list_node_parent((node, parent_type, node_name))        ((parent_type *)(node ? parent_addr(node, parent_type, node_name):0))   // 更改指针的指向类型 然后进行管理

#endif
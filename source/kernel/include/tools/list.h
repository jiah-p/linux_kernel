#ifndef __LIST_H__
#define __LIST_H__

#include "comm/types.h"

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

#endif
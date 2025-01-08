#include "tools/list.h"

void list_init(list_t * list){
    list->first = (list_node_t *) 0;
    list->last = (list_node_t *) 0;
    list->count = 0;
}

// 头插
void list_insert_first(list_t * list, list_node_t * node){
    node->next = list->first;
    node->pre = (list_node_t *)0;

    // 是否为空
    if(!list_empty(list)){
        list->first->pre = node;
        list->first = node;
    }else{
        list->first = list->last = node;
    }
    list->count++;
    
}

// 尾插
void list_insert_last(list_t * list, list_node_t * node){
    node->pre = list->last;
    node->next = (list_node_t *)0;

    if(!list_empty(list)){
        // 非空
        list->last->next = node;
        list->last = node;
    }else{
        list->first = list->last = node;
    }
    
    list->count++;
}

// 删除链头
list_node_t * list_remove_first(list_t * list){
    if(list_empty(list))
        return (list_node_t *)0;
    else{
        list_node_t * ret = list->first;
        list->first = ret->next; 
        // 链表为空
        if(--list->count == 0)
            list->last = list->first = (list_node_t *)0;
        else{
            ret->next->pre = (list_node_t *)0;   
        }
        ret->next = ret->pre = (list_node_t *)0; 
        return ret;
    }
}   

// 删除指定结点
list_node_t * list_remove(list_t * list, list_node_t * node){
    // 头结点
    if(node == list->first)
        return list_remove_first(list);
    // 尾结点 至少2个以上结点
    else if (node == list->last){
        node->pre->next = (list_node_t *)0;
        list->last = node->pre;
    }
    // 中间结点
    else{
        node->next->pre = node->pre;
        node->pre->next = node->next;     
    }

    node->next = node->pre = (list_node_t * )0;
    list->count--;

    return node; 
}
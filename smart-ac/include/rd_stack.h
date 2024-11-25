#ifndef __STACK_H__
#define __STACK_H__
#include <stdio.h>
#include <stdlib.h>

typedef struct StkItem
{
    void *ptr;
    int path;
    int connector;
} StkItem;

typedef struct Stack
{
    int top;        // 栈顶指针
    int capacity;   // 栈的容量
    StkItem *array; // 存储元素的数组
} Stack;

StkItem rd_stk_pop(Stack *stack);
void rd_stk_push(Stack *stack, StkItem item);
int rd_stk_is_empty(Stack *stack);
Stack *rd_stk_create_stack(int capacity);

#endif /*__STACK_H__*/
#ifndef __HASHMAP_H__
#define __HASHMAP_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 定义键值对结构体
typedef struct KeyValuePair
{
    char key[50];
    void *value;
} KeyValuePair;

// 定义哈希表结构体
typedef struct HashMap
{
    KeyValuePair *table;
    int count;
} HashMap;

#endif /* __HASHMAP_H__ */
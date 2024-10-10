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
HashMap *initHashMap();
bool map_insert(HashMap *hashMap, const char *key, void* value);
void *map_search(HashMap *hashMap, const char *key);
bool map_remove(HashMap *hashMap, const char *key);
#endif /* __HASHMAP_H__ */
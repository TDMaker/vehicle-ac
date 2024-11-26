#include <rd_hashmap.h>

#define TABLE_SIZE 1024 // 哈希表的大小

// 初始化哈希表
HashMap *initHashMap()
{
    HashMap *hashMap = (HashMap *)malloc(sizeof(HashMap));
    if (hashMap == NULL)
    {
        puts("HashMap inits failed!");
        exit(-1);
    }
    hashMap->table = (KeyValuePair *)calloc(TABLE_SIZE, sizeof(KeyValuePair));
    hashMap->count = 0;
    return hashMap;
}

// 哈希函数
unsigned int hashFunction(const char *key)
{
    unsigned int hash = 5381; // 初始哈希值
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TABLE_SIZE; // 取模运算得到索引
}

// 插入键值对
bool map_insert(HashMap *hashMap, const char *key, void* value)
{
    if (hashMap->count >= TABLE_SIZE)
    {
        return false; // 哈希表已满
    }
    unsigned int index = hashFunction(key);
    while (hashMap->table[index].value != 0 && strcmp(hashMap->table[index].key, "") != 0)
    {
        index = (index + 1) % TABLE_SIZE; // 线性探测
    }
    strcpy(hashMap->table[index].key, key);
    hashMap->table[index].value = value;
    hashMap->count++;
    return true;
}

// 查找键值对
void *map_search(HashMap *hashMap, const char *key)
{
    unsigned int index = hashFunction(key);
    do
    {
        if (strcmp(hashMap->table[index].key, key) == 0)
        {
            return hashMap->table[index].value;
        }
    } while ((index = (index + 1) % TABLE_SIZE) != hashFunction(key));
    return NULL; // 键不存在
}

// 删除键值对
bool map_remove(HashMap *hashMap, const char *key)
{
    unsigned int index = hashFunction(key);
    do
    {
        if (strcmp(hashMap->table[index].key, key) == 0)
        {
            hashMap->table[index].value = 0;
            strcpy(hashMap->table[index].key, "");
            hashMap->count--;
            return true;
        }
    } while ((index = (index + 1) % TABLE_SIZE) != hashFunction(key));
    return false; // 键不存在
}

bool map_update(HashMap *hashMap, const char *key, void* newValue)
{
    // 计算键的哈希值
    unsigned int index = hashFunction(key);

    // 查找键的位置
    while (hashMap->table[index].value != 0 && strcmp(hashMap->table[index].key, key) != 0)
    {
        index = (index + 1) % TABLE_SIZE; // 线性探测
    }

    // 如果找到了键，就更新它的值
    if (strcmp(hashMap->table[index].key, key) == 0)
    {
        hashMap->table[index].value = newValue;
        return true;
    }
    else if (hashMap->table[index].value == 0)
    {
        // 如果没找到键，但找到了空位置，相当于插入新键值对
        strcpy(hashMap->table[index].key, key);
        hashMap->table[index].value = newValue;
        hashMap->count++; // 插入新键值对时增加计数
        return true;
    }

    // 如果遍历完整个哈希表都没有找到键，也没有空位置，则无法更新
    return false;
}

// 释放哈希表
void freeHashMap(HashMap *hashMap)
{
    free(hashMap->table);
    free(hashMap);
}

// 测试函数
void testHashMap()
{
    // HashMap *hashMap = initHashMap();

    // // 插入键值对
    // map_insert(hashMap, "one", 1);
    // map_insert(hashMap, "two", 2);
    // map_insert(hashMap, "three", 3);

    // // 查找键值对
    // printf("Value of 'one': %d\n", map_search(hashMap, "one"));
    // printf("Value of 'two': %d\n", map_search(hashMap, "two"));
    // printf("Value of 'three': %d\n", map_search(hashMap, "three"));

    // // 删除键值对
    // bool result = map_remove(hashMap, "two");
    // if (result)
    // {
    //     printf("Key 'two' removed successfully.\n");
    // }
    // else
    // {
    //     printf("Key 'two' not found.\n");
    // }

    // // 再次查找
    // printf("Value of 'two' after removal: %d\n", map_search(hashMap, "two"));

    // // 释放哈希表
    // freeHashMap(hashMap);
}
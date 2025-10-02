// 调库
#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "table.h"

#define T Table_T

// 定义表格结构体
struct T
{
    // 该参数为散列表大小
    int size;
    // 该参数为比较函数指针
    int (*cmp)(const void *x, const void *y);
    // 该参数为哈希函数指针
    unsigned (*hash)(const void *key);
    // 该参数为链表结构体散列表
    struct binding
    {
        // 该参数指向下一节点
        struct binding *link;
        // 该参数指向键信息
        const void *key;
        // 该参数指向值信息
        void *value;
    } **buckets;
    // 该参数为键值对的数量
    int length;
    // 该参数为时间戳
    unsigned timestamp;
};

// 定义静态原子比较函数
static int cmpatom(const void *x, const void *y)
{
    return x != y;
}

// 定义静态原子哈希函数
static int unsigned hashatom(const void *key)
{
    return (unsigned long)key >> 2;
}

// 定义表格创建函数
T Table_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *key))
{
    // 声明新表格
    T table;
    // 声明计数变量
    int i;
    // 定义静态质数表
    static int primes[] =
        {
            509,
            509,
            1021,
            2053,
            4093,
            8191,
            16381,
            32771,
            65521,
            INT_MAX};

    // 判断假定数量是否合法
    assert(hint >= 0);
    // 在质数表中找到刚好大于假定数量的质数
    for (i = 1; primes[i] < hint; i++)
        ;
    // 分配内存空间，包括表头和稍小于所需的散列空间
    table = ALLOC(sizeof(*table) + primes[i - 1] * sizeof(table->buckets[0]));
    // 更新表格的大小
    table->size = primes[i - 1];
    // 判断是否有传入的比较函数，否则使用静态原子比较函数
    table->cmp = cmp ? cmp : cmpatom;
    // 判断是否有传入的哈希函数，否则使用静态原子哈希函数
    table->hash = hash ? hash : hashatom;
    // 更新表格内散列表
    table->buckets = (struct binding **)(table + 1);
    // 初始化散列表内容为空
    for (i = 0; i < table->size; i++)
        table->buckets[i] = NULL;
    // 更新表格的数量
    table->length = 0;
    // 更新表格的时间戳
    table->timestamp = 0;

    // 返回表格
    return table;
}

// 定义表格释放函数
void Table_free(T *table)
{
    // 判断表格指针和表格是否合法
    assert(table && *table);
    // 判断表格内是否有内容
    if ((*table)->length > 0)
    {
        // 声明计数变量
        int i;
        // 声明链表节点结构体指针
        struct binding *p, *q;

        // 遍历整个表格散列表
        for (i = 0; i < (*table)->size; i++)
            // 遍历整条链表
            for (p = (*table)->buckets[i]; p; p = q)
            {
                // 记录下一节点
                q = p->link;
                // 释放当前节点
                FREE(p);
            }
    }
    // 释放表格
    FREE(*table);
}

// 定义表格计数函数
int Table_length(T table)
{
    // 判断表格是否合法
    assert(table);

    // 返回表格内键值对的数量
    return table->length;
}

// 定义表格添加函数
void *Table_put(T table, const void *key, void *value)
{
    // 声明计数变量
    int i;
    // 声明链表节点结构体指针
    struct binding *p;
    // 声明前置值
    void *prev;

    // 判断表格是否合法
    assert(table);
    // 判断键是否合法
    assert(key);
    // 计算哈希值作为存储位置
    i = (*table->hash)(key) % table->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现键相同时停止
    for (p = table->buckets[i]; p; p = p->link)
        if ((*table->cmp)(key, p->key) == 0)
            break;
    // 判断是否有相同键
    if (p == NULL)
    {
        // 分配内存空间
        NEW(p);
        // 更新键内容
        p->key = key;
        // 将当前节点放在链表起始
        p->link = table->buckets[i];
        table->buckets[i] = p;
        // 表格内键值对数量增加
        table->length++;
        // 因为没有相同键，所有前置值为空
        prev = NULL;
    }
    else
        // 更新前置值
        prev = p->value;
    // 更新键值对的值
    p->value = value;
    // 更新时间戳
    table->timestamp++;

    // 返回前置值
    return prev;
}

// 定义表格查找函数
void *Table_get(T table, const void *key)
{
    // 声明计数变量
    int i;
    // 声明链表节点结构体指针
    struct binding *p;

    // 判断表格是否合法
    assert(table);
    // 判断键是否合法
    assert(key);
    // 计算哈希值作为存储位置
    i = (*table->hash)(key) % table->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现键相同时停止
    for (p = table->buckets[i]; p; p = p->link)
        if ((*table->cmp)(key, p->key) == 0)
            break;

    // 返回所需的值
    return p ? p->value : NULL;
}

// 定义表格删除函数
void *Table_remove(T table, const void *key)
{
    // 声明计数变量
    int i;
    // 声明散列表指针
    struct binding **pp;

    // 判断表格是否合法
    assert(table);
    // 判断键是否合法
    assert(key);
    // 更新时间戳
    table->timestamp++;
    // 计算哈希值作为存储位置
    i = (*table->hash)(key) % table->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现键相同时停止
    for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
        if ((*table->cmp)(key, (*pp)->key) == 0)
        {
            // 定义链表节点结构体指针
            struct binding *p = *pp;
            // 定义并记录键值对的值
            void *value = p->value;
            // 将后面的节点替换此节点
            *pp = p->link;
            // 释放所需节点
            FREE(p);
            // 表格内键值对数量减少
            table->length--;

            // 返回键值对的值
            return value;
        }

    // 删除失败返回空
    return NULL;
}

// 定义表格遍历调用函数
void Table_map(T table, void apply(const void *key, void **value, void *cl), void *cl)
{
    // 声明计数变量
    int i;
    // 声明时间戳变量
    unsigned stamp;
    // 声明链表节点结构体指针
    struct binding *p;

    // 判断表格是否合法
    assert(table);
    // 判断回调函数是否合法
    assert(apply);
    // 记录当前时间戳
    stamp = table->timestamp;
    // 遍历整个散列表
    for (i = 0; i < table->size; i++)
        // 遍历整条链表
        for (p = table->buckets[i]; p; p = p->link)
        {
            // 将各节点内键值对作为参数调用回调函数
            apply(p->key, &p->value, cl);
            // 判断时间戳是否变化，即防止在调用回调函数中出现增添、修改和删除表格中的内容
            assert(table->timestamp == stamp);
        }
}

// 定义表格转数组函数
void **Table_toArray(T table, void *end)
{
    // 声明和定义计数变量
    int i, j = 0;
    // 声明指针数组
    void **array;
    // 声明链表节点结构体指针
    struct binding *p;

    // 判断表格是否合法
    assert(table);
    // 分配内存空间
    array = ALLOC((2 * table->length + 1) * sizeof(*array));
    // 遍历整个散列表
    for (i = 0; i < table->size; i++)
        // 遍历整条链表
        for (p = table->buckets[i]; p; p = p->link)
        {
            // 偶数位存储键信息
            array[j++] = (void *)p->key;
            // 奇数位存储值信息
            array[j++] = p->value;
        }
    // 更新数组最后一位位传入的末尾参数
    array[j] = end;

    // 返回数组
    return array;
}

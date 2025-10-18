// 调库
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "ring.h"
#include "mem.h"

#define T Ring_T

// 定义环节点结构体
struct node
{
    // 两参数分别为左侧节点指针和右侧节点指针
    struct node *llink, *rlink;
    // 该参数为该节点的内容指针
    void *value;
};

// 定义环结构体
struct T
{
    // 该参数为环上头节点结构体指针
    struct node *head;
    // 该参数为环长度
    int length;
};

// 定义环新建函数
T Ring_new(void)
{
    // 定义环结构体指针
    T ring;

    // 分配内存空间
    NEW0(ring);
    // 将环上头节点指针指向空
    ring->head = NULL;

    // 返回环
    return ring;
}

// 定义环创建函数
T Ring_ring(void *x, ...)
{
    // 声明可变参数列表
    va_list ap;
    // 新建环
    T ring = Ring_new();

    // 初始化可变参数列表
    va_start(ap, x);
    // 遍历整个参数列表
    for(; x; x = va_arg(ap, void *))
        // 添加元素到环末尾
        Ring_addhi(ring, x);
    // 结束可变参数列表的访问
    va_end(ap);

    // 返回环
    return ring;
}

// 定义环释放函数
void Ring_free(T *ring)
{
    // 声明环节点结构体指针
    struct node *p, *q;

    // 判断环指针和环是否合法
    assert(ring && *ring);
    // 判断环是否为空
    if((p = (*ring)->head) != NULL)
    {
        // 定义计数变量并记录环长度
        int n = (*ring)->length;

        // 遍历释放每一个环节点
        for(; n-- > 0; p = q)
        {
            q = p->rlink;
            FREE(p);
        }
    }
    // 释放环
    FREE(*ring);
}

// 定义环计数函数
int Ring_length(T ring)
{
    // 判断环是否合法
    assert(ring);

    // 返回序列成员数量
    return ring->length;
}

// 定义环查找函数
void *Ring_get(T ring, int i)
{
    // 声明环节点结构体指针
    struct node *q;

    // 判断环是否合法
    assert(ring);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < ring->length);
    {
        // 声明计数变量
        int n;
        // 记录头节点
        q = ring->head;

        // 根据索引值判断向右还是向左查找
        if(i <= ring->length / 2)
            // 向右查找
            for(n = i; n-- > 0; )
                q = q->rlink;
        else
            // 向左查找
            for(n = ring->length - i; n-- > 0; )
                q = q->llink;
    }

    // 返回节点内容
    return q->value;
}

// 定义环修改函数
void *Ring_put(T ring, int i, void *x)
{
    // 声明环节点结构体指针
    struct node *q;
    // 声明内容指针
    void *prev;

    // 判断环是否合法
    assert(ring);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < ring->length);
    {
        // 声明计数变量
        int n;
        // 记录头节点
        q = ring->head;

        // 根据索引值判断向右还是向左查找
        if(i <= ring->length / 2)
            // 向右查找
            for(n = i; n-- > 0; )
                q = q->rlink;
        else
            // 向左查找
            for(n = ring->length - i; n-- > 0; )
                q = q->llink;
    }
    // 记录节点前置内容
    prev = q->value;
    // 更新节点内容
    q->value = x;

    // 返回前置内容指针
    return prev;
}

// 定义环添加函数
void *Ring_add(T ring, int pos, void *x)
{
    // 判断环是否合法
    assert(ring);
    // 判断位置参数是否在范围内，这里允许位置参数出现负值
    assert(pos >= -ring->length && pos <= ring->length + 1);
    // 若要在头节点位置添加，调用环低端添加函数
    if(pos == 1 || pos == -ring->length)
        return Ring_addlo(ring, x);
    else
    {
        // 声明环节点结构体指针
        struct node *p, *q;
        // 修正位置参数为索引值
        int i = pos < 0 ? pos + ring->length : pos - 1;
        {
            // 声明计数变量
            int n;
            // 记录头节点
            q = ring->head;

            // 根据索引值判断向右还是向左查找
            if(i <= ring->length / 2)
                // 向右查找
                for(n = i; n-- > 0; )
                    q = q->rlink;
            else
                // 向左查找
                for(n = ring->length - i; n-- > 0; )
                    q = q->llink;
        }
        // 分配新节点结构体内存
        NEW(p);
        // 将节点插入
        {
            // 更新插入节点的左节点指针
            p->llink = q->llink;
            // 更新左侧节点的右节点指针
            q->llink->rlink = p;
            // 更新插入节点的右节点指针
            p->rlink = q;
            // 更新右侧节点的左节点指针
            q->llink = p;
        }
        // 环长度增加
        ring->length++;

        // 更新并返回插入节点内容
        return p->value = x;
    }
}

// 定义环删除函数
void *Ring_remove(T ring, int i)
{
    // 声明内容指针
    void *x;
    // 声明环节点结构体指针
    struct node *q;

    // 判断环是否合法
    assert(ring);
    // 判断环是否为空
    assert(ring->length > 0);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < ring->length);
    {
        // 声明计数变量
        int n;
        // 记录头节点
        q = ring->head;

        // 根据索引值判断向右还是向左查找
        if(i <= ring->length / 2)
            // 向右查找
            for(n = i; n-- > 0; )
                q = q->rlink;
        else
            // 向左查找
            for(n = ring->length - i; n-- > 0; )
                q = q->llink;
    }
    // 若需删除节点为头节点，则向右更改头节点
    if(i == 0)
        ring->head = ring->head->rlink;
    // 记录删除节点内容
    x = q->value;
    // 将节点从环中剔除
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    // 释放环节点
    FREE(q);
    // 判断是否将环长度清空，若是则将头节点指针指向空
    if(--ring->length == 0)
        ring->head = NULL;

    // 返回内容指针
    return x;
}

// 定义环低端添加函数
void *Ring_addlo(T ring, void *x)
{
    // 判断环是否合法
    assert(ring);
    // 调用环高端添加函数
    Ring_addhi(ring, x);
    // 向左移动头节点位置
    ring->head = ring->head->llink;

    // 返回内容指针
    return x;
}

// 定义环高端添加函数
void *Ring_addhi(T ring, void *x)
{
    // 声明环节点结构体指针
    struct node *p, *q;

    // 判断环是否合法
    assert(ring);
    // 分配内存空间
    NEW(p);
    // 存储环头节点并判断环头节点是否为空
    if((q = ring->head) != NULL)
    {
        // 更新插入节点的左节点指针
        p->llink = q->llink;
        // 更新左侧节点的右节点指针
        q->llink->rlink = p;
        // 更新插入节点的右节点指针
        p->rlink = q;
        // 更新右侧节点的左节点指针
        q->llink = p;
    }
    else
        // 若环为空时插入节点
        ring->head = p->llink = p->rlink = p;
    // 环长度增加
    ring->length++;

    // 存入并返回节点内容
    return p->value = x;
}

// 定义环低端删除函数
void *Ring_remlo(T ring)
{
    // 判断环是否合法
    assert(ring);
    // 判断环是否为空
    assert(ring->length > 0);
    // 向右移动头节点位置
    ring->head = ring->head->rlink;

    // 调用环高端删除函数
    return Ring_remhi(ring);
}

// 定义环高端删除函数
void *Ring_remhi(T ring)
{
    // 声明内容指针
    void *x;
    // 声明环节点结构体指针
    struct node *q;

    // 判断环是否合法
    assert(ring);
    // 判断环是否为空
    assert(ring->length > 0);
    // 存储最高位节点
    q = ring->head->llink;
    // 存储最高位节点内容
    x = q->value;
    // 将节点从环中剔除
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    // 释放内容空间
    FREE(q);
    // 判断是否将环长度清空，若是则将头节点指针指向空
    if(--ring->length == 0)
        ring->head = NULL;

    // 返回内容指针
    return x;
}

// 定义环旋转函数
void *Ring_rotate(T ring, int n)
{
    // 声明环节点结构体指针
    struct node *q;
    // 声明计数变量
    int i;

    // 判断环是否合法
    assert(ring);
    // 判断旋转次数是否在范围内，这里允许旋转次数出现负值
    assert(n >= -ring->length && n <= ring->length);

    // 修正并存储向右旋转次数
    if(n >= 0)
        i = n % ring->length;
    else
        i = n + ring->length;
    {
        // 声明计数变量
        int n;
        // 记录头节点
        q = ring->head;

        // 根据索引值判断向右还是向左查找
        if(i <= ring->length / 2)
            // 向右查找
            for(n = i; n-- > 0; )
                q = q->rlink;
        else
            // 向左查找
            for(n = ring->length - i; n-- > 0; )
                q = q->llink;
    }
    // 更新环头节点
    ring->head = q;
}

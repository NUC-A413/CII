// 调库
#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "arith.h"
#include "set.h"

#define T Set_T

// 定义集合结构体
struct T
{
    // 该参数为成员的数量
    int length;
    // 该参数为时间戳
    unsigned timestamp;
    // 该参数为比较函数指针
    int (*cmp)(const void *x, const void *y);
    // 该参数为哈希函数指针
    unsigned (*hash)(const void *x);
    // 该参数为散列表大小
    int size;
    // 该参数为链表结构体散列表
    struct member
    {
        // 该参数指向下一节点
        struct member *link;
        // 该参数指向成员信息
        const void *member;
    } **buckets;
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

// 定义静态集合拷贝函数
static T copy(T t, int hint)
{
    // 声明集合结构体指针
    T set;

    // 判断集合是否合法
    assert(t);
    // 新建集合
    set = Set_new(hint, t->cmp, t->hash);
    {
        // 声明计数变量
        int i;
        // 声明链表结构体指针
        struct member *q;

        // 遍历整个集合散列表
        for(i = 0; i < t->size; i++)
            // 遍历整条链表
            for(q = t->buckets[i]; q; q = q->link)
            {
                // 声明链表结构体指针
                struct member *p;
                // 记录当前节点成员值
                const void *member = q->member;
                // 计算哈希值作为存储位置
                int i = (*set->hash)(member) % set->size;
                // 分配内存空间
                NEW(p);
                // 更新成员值
                p->member = member;
                // 将当前节点放在链表起始
                p->link = set->buckets[i];
                set->buckets[i] = p;
                // 集合内成员数量增加
                set->length++;
            }
    }

    // 返回集合
    return set;
}

// 定义集合创建函数
T Set_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *x))
{
    // 声明新集合
    T set;
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
        INT_MAX
    };

    // 判断假定数量是否合法
    assert(hint >= 0);
    // 在质数表中找到刚好大于假定数量的质数
    for(i = 1; primes[i] < hint; i++);
    // 分配内存空间，包括表头和稍小于所需的散列空间
    set = ALLOC(sizeof(*set) + primes[i - 1] * sizeof(set->buckets[0]));
    // 更新集合的大小
    set->size = primes[i - 1];
    // 判断是否有传入的比较函数，否则使用静态原子比较函数
    set->cmp = cmp ? cmp : cmpatom;
    // 判断是否有传入的哈希函数，否则使用静态原子哈希函数
    set->hash = hash ? hash : hashatom;
    // 更新集合内散列表
    set->buckets = (struct member **)(set + 1);
    // 初始化散列表内容为空
    for(i = 0; i < set->size; i++)
        set->buckets[i] = NULL;
    // 更新集合的数量
    set->length = 0;
    // 更新集合的时间戳
    set->timestamp = 0;

    return set;
}

// 定义集合释放函数
void Set_free(T *set)
{
    // 判断集合指针和集合是否合法
    assert(set && *set);
    // 判断集合内是否有内容
    if((*set)->length > 0)
    {
        // 声明计数变量
        int i;
        // 声明链表节点结构体指针
        struct member *p, *q;

        // 遍历整个集合散列表
        for(i = 0; i < (*set)->size; i++)
            // 遍历整条链表
            for(p = (*set)->buckets[i]; p; p = q)
            {
                // 记录下一节点
                q = p->link;
                // 释放当前节点
                FREE(p);
            }
    }
    // 释放集合
    FREE(*set);
}

// 定义集合计数函数
int Set_length(T set)
{
    // 判断集合是否合法
    assert(set);

    // 返回集合内成员的数量
    return set->length;
}

// 定义集合查找函数
int Set_member(T set, const void *member)
{
    // 声明计数变量
    int i;
    // 声明链表节点结构体指针
    struct member *p;

    // 判断集合是否合法
    assert(set);
    // 判断成员值是否合法
    assert(member);
    // 计算哈希值作为存储位置
    i = (*set->hash)(member) % set->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现成员值相同时停止
    for(p = set->buckets[i]; p; p = p->link)
        if((*set->cmp)(member, p->member) == 0)
            break;

    // 返回成员值是否存在
    return p != NULL;
}

// 定义集合添加函数
void Set_put(T set, const void *member)
{
    // 声明计数变量
    int i;
    // 声明链表节点结构体指针
    struct member *p;

    // 判断集合是否合法
    assert(set);
    // 判断成员值是否合法
    assert(member);
    // 计算哈希值作为存储位置
    i = (*set->hash)(member) % set->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现成员值相同时停止
    for(p = set->buckets[i]; p; p = p->link)
        if((*set->cmp)(member, p->member) == 0)
            break;
    // 判断是否有相同成员
    if(p == NULL)
    {
        // 分配内存空间
        NEW(p);
        // 更新成员值
        p->member = member;
        // 将当前节点放在链表起始
        p->link = set->buckets[i];
        set->buckets[i] = p;
        // 集合内键值对数量增加
        set->length++;
    }
    else
        // 更新成员值
        p->member = member;
    // 更新时间戳
    set->timestamp++;
}

// 定义集合删除函数
void *Set_remove(T set, const void *member)
{
    // 声明计数变量
    int i;
    // 声明散列表指针
    struct member **pp;

    // 判断集合是否合法
    assert(set);
    // 判断成员值是否合法
    assert(member);
    // 更新时间戳
    set->timestamp++;
    // 计算哈希值作为存储位置
    i = (*set->hash)(member) % set->size;
    // 将链表节点结构体指针沿链表逐个比较，当出现成员值相同时停止
    for(pp = &set->buckets[i]; *pp; pp = &(*pp)->link)
        if((*set->cmp)(member, (*pp)->member) == 0)
        {
            // 定义链表节点结构体指针
            struct member *p = *pp;
            // 将后面的节点替换此节点
            *pp = p->link;
            // 记录成员值
            member = p->member;
            // 释放所需节点
            FREE(p);
            // 集合内成员数量减少
            set->length--;

            // 返回成员值
            return (void *)member;
        }

    // 删除失败返回空
    return NULL;
}

// 定义集合并集函数
T Set_union(T s, T t)
{
    // 判断集合s是否为空
    if(s == NULL)
    {
        // 判断集合t是否合法
        assert(t);
        // 返回集合t的拷贝集合
        return copy(t, t->size);
    }
    // 判断集合t是否为空
    else if(t == NULL)
    {
        // 返回集合s的拷贝集合
        return copy(s, s->size);
    }
    else
    {
        // 拷贝集合s，大小取两个集合中最大的
        T set = copy(s, Arith_max(s->size, t->size));

        // 判断两个集合的比较函数与哈希函数是否相同
        assert(s->cmp == t->cmp && s->hash == t->hash);
        {
            // 声明计数变量
            int i;
            // 声明链表节点结构体指针
            struct member *q;

            // 添加整个集合t成员
            for(i = 0; i < t->size; i++)
                for(q = t->buckets[i]; q; q = q->link)
                    Set_put(set, q->member);
        }

        // 返回新集合
        return set;
    }
}

// 定义集合交集函数
T Set_inter(T s, T t)
{
    // 判断集合s是否为空
    if(s == NULL)
    {
        // 判断集合t是否合法
        assert(t);
        // 返回空集合，大小和函数取集合t的
        return Set_new(t->size, t->cmp, t->hash);
    }
    // 判断集合t是否为空
    else if(t == NULL)
        // 返回空集合，大小和函数取集合s的
        return Set_new(s->size, s->cmp, s->hash);
    // 比较两个集合大小，调换参数递归
    else if(s->length < t->length)
        return Set_inter(t, s);
    else
    {
        // 新建集合，大小取两个集合中最小的，函数取集合s的
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);

        // 判断两个集合的比较函数与哈希函数是否相同
        assert(s->cmp = t->cmp && s->hash == t->hash);
        {
            // 声明计数变量
            int i;
            // 声明链表节点结构体指针
            struct member *q;

            // 遍历整个集合t，若有成员在集合s中存在，则添加至新集合
            for(i = 0; i < t->size; i++)
                for(q = t->buckets[i]; q; q = q->link)
                    if(Set_member(s, q->member))
                    {
                        // 声明链表结构体指针
                        struct member *p;
                        // 记录当前节点成员值
                        const void *member = q->member;
                        // 计算哈希值作为存储位置
                        int i = (*set->hash)(member) % set->size;
                        // 分配内存空间
                        NEW(p);
                        // 更新成员值
                        p->member = member;
                        // 将当前节点放在链表起始
                        p->link = set->buckets[i];
                        set->buckets[i] = p;
                        // 集合内成员数量增加
                        set->length++;
                    }
        }

        // 返回新集合
        return set;
    }
}

// 定义集合差集函数（原文中写反了，我做了修改）
T Set_minus(T s, T t)
{
    // 判断集合s是否为空
    if(s == NULL)
    {
        // 判断集合t是否合法
        assert(t);
        // 返回空集合，大小和函数取集合t的
        return Set_new(t->size, t->cmp, t->hash);
    }
    // 判断集合t是否为空
    else if(t = NULL)
        // 返回集合s的拷贝集合
        return copy(s, s->size);
    else
    {
        // 新建集合，大小取两个集合中最小的，函数取集合s的
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);

        // 判断两个集合的比较函数与哈希函数是否相同
        assert(s->cmp = t->cmp && s->hash == t->hash);
        {
            // 声明计数变量
            int i;
            // 声明链表节点结构体指针
            struct member *q;

            // 遍历整个集合s，若有成员在集合t中不存在，则添加至新集合
            for(i = 0; i < s->size; i++)
                for(q = s->buckets[i]; q; q = q->link)
                    if(!Set_member(t, q->member))
                    {
                        // 声明链表结构体指针
                        struct member *p;
                        // 记录当前节点成员值
                        const void *member = q->member;
                        // 计算哈希值作为存储位置
                        int i = (*set->hash)(member) % set->size;
                        // 分配内存空间
                        NEW(p);
                        // 更新成员值
                        p->member = member;
                        // 将当前节点放在链表起始
                        p->link = set->buckets[i];
                        set->buckets[i] = p;
                        // 集合内成员数量增加
                        set->length++;
                    }
        }

        // 返回新集合
        return set;
    }
}

// 定义集合对称差集函数
T Set_diff(T s, T t)
{
    // 判断集合s是否为空
    if(s == NULL)
    {
        // 判断集合t是否合法
        assert(t);
        // 返回集合t的拷贝集合
        return copy(t, t->size);
    }
    // 判断集合t是否为空
    else if(t == NULL)
        // 返回集合s的拷贝集合
        return copy(s, s->size);
    else
    {
        // 新建集合，大小取两个集合中最小的，函数取集合s的
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);

        // 判断两个集合的比较函数与哈希函数是否相同
        assert(s->cmp = t->cmp && s->hash == t->hash);
        {
            // 声明计数变量
            int i;
            // 声明链表节点结构体指针
            struct member *q;

            // 遍历整个集合t，若有成员在集合s中不存在，则添加至新集合
            for(i = 0; i < t->size; i++)
                for(q = t->buckets[i]; q; q = q->link)
                    if(!Set_member(s, q->member))
                    {
                        // 声明链表结构体指针
                        struct member *p;
                        // 记录当前节点成员值
                        const void *member = q->member;
                        // 计算哈希值作为存储位置
                        int i = (*set->hash)(member) % set->size;
                        // 分配内存空间
                        NEW(p);
                        // 更新成员值
                        p->member = member;
                        // 将当前节点放在链表起始
                        p->link = set->buckets[i];
                        set->buckets[i] = p;
                        // 集合内成员数量增加
                        set->length++;
                    }
        }
        {
            // 交换两个集合（因为是形参，所以并不会实际交换）
            T u = t;
            t = s;
            s = u;
        }
        {
            // 声明计数变量
            int i;
            // 声明链表节点结构体指针
            struct member *q;

            // 遍历整个集合t，若有成员在集合s中不存在，则添加至新集合
            for(i = 0; i < t->size; i++)
                for(q = t->buckets[i]; q; q = q->link)
                    if(!Set_member(s, q->member))
                    {
                        // 声明链表结构体指针
                        struct member *p;
                        // 记录当前节点成员值
                        const void *member = q->member;
                        // 计算哈希值作为存储位置
                        int i = (*set->hash)(member) % set->size;
                        // 分配内存空间
                        NEW(p);
                        // 更新成员值
                        p->member = member;
                        // 将当前节点放在链表起始
                        p->link = set->buckets[i];
                        set->buckets[i] = p;
                        // 集合内成员数量增加
                        set->length++;
                    }
        }

        // 返回新集合
        return set;
    }
    // 这里实现略显麻烦，如果用上面写好的并集和差集函数来实现会比较简单
}

// 定义集合遍历调用函数
void Set_map(T set, void apply(const void *member, void *cl), void *cl)
{
    // 声明计数变量
    int i;
    // 声明时间戳变量
    unsigned stamp;
    // 声明链表节点结构体指针
    struct member *p;

    // 判断集合是否合法
    assert(set);
    // 判断回调函数是否合法
    assert(apply);
    // 记录当前时间戳
    stamp = set->timestamp;
    // 遍历整个散列表
    for(i = 0; i < set->size; i++)
        // 遍历整条链表
        for(p = set->buckets[i]; p; p = p->link)
        {
            // 将各节点内成员值作为参数调用回调函数
            apply(p->member, cl);
            // 判断时间戳是否变化，即防止在调用回调函数中出现增添、修改和删除集合中的内容
            assert(set->timestamp == stamp);
        }
}

// 定义集合转数组函数
void **Set_toArray(T set, void *end)
{
    // 声明和定义计数变量
    int i, j = 0;
    // 声明指针数组
    void **array;
    // 声明链表节点结构体指针
    struct member *p;

    // 判断集合是否合法
    assert(set);
    // 分配内存空间
    array = ALLOC((set->length + 1) * sizeof(*array));
    // 遍历整个散列表
    for(i = 0; i < set->size; i++)
        // 遍历整条链表
        for(p = set->buckets[i]; p; p = p->link)
            // 存储成员值
            array[j++] = (void *)p->member;
    // 更新数组最后一位位传入的末尾参数
    array[j] = end;

    // 返回数组
    return array;
}

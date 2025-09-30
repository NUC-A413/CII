// 调库
#include <stdarg.h>
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "list.h"

#define T List_T

// 定义链表追加函数
T List_append(T list, T tail)
{
    // 定义链表节点结构体二级指针存储被追加的链表
    T *p = &list;

    // 让链表节点结构体二级指针指向被追加链表的最后一个节点指向的下一节点
    while (*p)
        p = &(*p)->rest;
    // 将要追加的链表挂在到被追加链表的后面
    *p = tail;

    // 返回被追加的链表
    return list;
}

// 定义链表拷贝函数
T List_copy(T list)
{
    // 定义新链表及其指针
    T head, *p = &head;

    // 循环新建新链表新节点并拷贝原链表中的内容到新链表
    for (; list; list = list->rest)
    {
        NEW(*p);
        (*p)->first = list->first;
        p = &(*p)->rest;
    }
    // 将新链表最后指向空值
    *p = NULL;

    // 返回新链表
    return head;
}

// 定义链表创建函数
T List_list(void *x, ...)
{
    // 声明可变参数列表
    va_list ap;
    // 定义新链表及其指针
    T list, *p = &list;

    // 初始化可变参数列表
    va_start(ap, x);
    // 遍历整个参数列表
    for (; x; x = va_arg(ap, void *))
    {
        // 新建链表节点
        NEW(*p);
        // 更新节点中内容参数
        (*p)->first = x;
        // 控制指向下一链表节点
        p = &(*p)->rest;
    }
    // 将新链表最后指向空值
    *p = NULL;
    // 结束可变参数列表的访问
    va_end(ap);

    return list;
}

// 定义链表弹栈函数
T List_pop(T list, void **x)
{
    // 判断链表是否存在，若不存在则直接返回原链表
    if (list)
    {
        // 定义链表节点指针指向链表第二个节点
        T head = list->rest;
        // 判断传参参数是否合法
        if (x)
            // 将第一个节点中的内容传给传参参数
            *x = list->first;
        // 释放第一节点，将第二节点作为新链表表头
        FREE(list);

        // 返回新链表
        return head;
    }
    else
    {
        return list;
    }
}

// 定义链表压栈函数
T List_push(T list, void *x)
{
    // 声明新链表节点指针
    T p;

    // 分配内存空间
    NEW(p);
    // 传入参数
    p->first = x;
    // 将新节点作为链表表头
    p->rest = list;

    // 返回新链表
    return p;
}

// 定义链表反转函数
T List_reverse(T list)
{
    // 定义链表表头指针和声明链表下一节点指针
    T head = NULL, next;

    // 遍历链表，将每个节点倒置
    for (; list; list = next)
    {
        // 记录下一节点
        next = list->rest;
        // 将当前节点指向指向前一节点实现倒置
        list->rest = head;
        // 更新表头指针位置至当前节点
        head = list;
        // 每次循环结束更新当前节点指针至下一节点
    }

    // 返回新链表
    return head;
}

// 定义链表计数函数
int List_length(T list)
{
    // 声明计数变量
    int n;

    // 遍历链表并计数（有多少个有效链表节点指针就有多少个节点）
    for (n = 0; list; list = list->rest)
        n++;

    // 返回链表计数
    return n;
}

// 定义链表释放函数
void List_free(T *list)
{
    // 声明链表下一节点指针
    T next;

    // 判断链表指针是否合法
    assert(list);
    // 遍历链表依次释放节点
    for (; *list; *list = next)
    {
        // 记录下一节点
        next = (*list)->rest;
        // 释放当前节点
        FREE(*list);
    }
}

// 定义链表遍历调用函数
void List_map(T list, void apply(void **x, void *cl), void *cl)
{
    // 判断回调函数是否合法
    assert(apply);
    // 遍历链表，将各节点内数据作为参数调用回调函数
    for (; list; list = list->rest)
        apply(&list->first, cl);
}

// 定义链表转数组函数
void **List_toArray(T list, void *end)
{
    // 声明计数变量和定义总数变量记录链表长度
    int i, n = List_length(list);
    // 分配内存空间
    // 因为数组成员都是一级指针，所以数组要定义为二级指针
    void **array = ALLOC((n + 1) * sizeof(*array));

    // 遍历链表存储各节点信息至数组
    for (i = 0; i < n; i++)
    {
        array[i] = list->first;
        list = list->rest;
    }
    // 更新数组最后一位位传入的末尾参数
    array[i] = end;

    // 返回数组
    return array;
}

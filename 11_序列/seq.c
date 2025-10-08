// 调库
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "seq.h"
#include "array.h"
#include "arrayrep.h"
#include "mem.h"

#define T Seq_T

// 定义序列结构体
struct T
{
    // 该参数为动态数组
    struct Array_T array;
    // 该参数为序列中值的数目
    int length;
    // 该参数为序列首元素在动态数组中的索引值
    int head;
};

// 定义静态序列扩展函数
static void expand(T seq)
{
    // 记录当前序列内动态数组长度
    int n = seq->array.length;

    // 更新动态数组长度为两倍
    Array_resize(&seq->array, 2 * n);
    // 判断序列元素起始位置是否大于零，若大于，需要重新拷贝
    if(seq->head > 0)
    {
        // 记录原序列首元素地址
        void **old = &((void **)seq->array.array)[seq->head];
        // 拷贝到新地址
        memcpy(old + n, old, (n - seq->head) * sizeof(void *));
        // 更新序列首元素索引值
        seq->head += n;
    }
}

// 定义序列新建函数
T Seq_new(int hint)
{
    // 声明序列结构体指针
    T seq;

    // 判断预期数量是否合法
    assert(hint >= 0);
    // 分配内存空间并初始化为零
    NEW0(seq);
    // 若预期数量为零则重置
    if(hint == 0)
        hint = 16;
    // 初始化动态数组
    ArrayRep_init(&seq->array, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));

    // 返回序列
    return seq;
}

// 定义序列创建函数
T Seq_seq(void *x, ...)
{
    // 声明可变参数列表
    va_list ap;
    // 新建序列
    T seq = Seq_new(0);

    // 初始化可变参数列表
    va_start(ap, x);
    // 遍历整个参数列表
    for(; x; x = va_arg(ap, void *))
        // 添加元素到序列末尾
        Seq_addhi(seq, x);
    // 结束可变参数列表的访问
    va_end(ap);

    // 返回序列
    return seq;
}

// 定义序列释放函数
void Seq_free(T *seq)
{
    // 判断序列指针和序列是否合法
    assert(seq && *seq);
    // 判断动态数组参数是否是序列内的第一个成员
    assert((void *)*seq == (void *)&(*seq)->array);
    // 释放动态数组
    Array_free((Array_T *)seq);
}

// 定义序列计数函数
int Seq_length(T seq)
{
    // 判断序列是否合法
    assert(seq);

    // 返回序列成员数量
    return seq->length;
}

// 定义序列查找函数
void *Seq_get(T seq, int i)
{
    // 判断序列是否合法
    assert(seq);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < seq->length);

    // 计算得到地址
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

// 定义序列添加函数
void *Seq_put(T seq, int i, void *x)
{
    // 声明前置内容指针
    void *prev;

    // 判断序列是否合法
    assert(seq);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < seq->length);
    // 记录前置内容
    prev = ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
    // 更新当前位置内容
    ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;

    // 返回前置内容指针
    return prev;
}

// 定义序列低端添加函数
void *Seq_addlo(T seq, void *x)
{
    // 定义计数变量
    int i = 0;

    // 判断序列是否合法
    assert(seq);
    // 当序列长度与动态数组长度相同时扩展动态数组长度
    if(seq->length == seq->array.length)
        expand(seq);
    // 更新首元素索引值
    if(--seq->head < 0)
        seq->head = seq->array.length - 1;
    // 序列长度增加
    seq->length++;

    // 计入元素并返回
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
}

// 定义序列高端添加函数
void *Seq_addhi(T seq, void *x)
{
    // 定义计数变量
    int i;

    // 判断序列是否合法
    assert(seq);
    // 当序列长度与动态数组长度相同时扩展动态数组长度
    if(seq->length == seq->array.length)
        expand(seq);
    // 序列长度增加
    i = seq->length++;

    // 计入元素并返回
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
}

// 定义序列低端删除函数
void *Seq_remlo(T seq)
{
    // 定义计数变量
    int i = 0;
    // 声明内容指针
    void *x;

    // 判断序列是否合法
    assert(seq);
    // 判断序列内是否为空
    assert(seq->length > 0);
    // 记录被删除内容
    x = ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
    // 更新首元素索引值
    seq->head = (seq->head + 1) % seq->array.length;
    // 序列长度减小
    --seq->length;

    // 返回内容指针
    return x;
}

// 定义序列高端删除函数
void *Seq_remhi(T seq)
{
    // 定义计数变量
    int i;

    // 判断序列是否合法
    assert(seq);
    // 判断序列内是否为空
    assert(seq->length > 0);
    // 序列长度减小并记录
    i = --seq->length;

    // 返回被删除内容
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

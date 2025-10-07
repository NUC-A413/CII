// 调库
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "array.h"
#include "arrayrep.h"
#include "mem.h"

#define T Array_T

// 定义动态数组创建函数
T Array_new(int length, int size)
{
    // 声明动态数组结构体指针
    T array;

    // 分配内存
    NEW(array);
    // 判断数组是否有长度，根据长度分配内存
    if(length > 0)
        ArrayRep_init(array, length, size, CALLOC(length, size));
    else
        ArrayRep_init(array, length, size, NULL);

    // 返回动态数组
    return array;
}

// 定义动态数组释放函数
void Array_free(T *array)
{
    // 判断动态数组指针和动态数组是否合法
    assert(array && *array);
    // 释放数组内存空间
    FREE((*array)->array);
    // 释放描述符内存空间
    FREE(*array);
}

// 定义动态数组计数函数
int Array_length(T array)
{
    // 判断动态数组是否合法
    assert(array);

    // 返回成员的数量
    return array->length;
}

// 定义动态数组计大小函数
int Array_size(T array)
{
    // 判断动态数组是否合法
    assert(array);

    // 返回成员的大小
    return array->size;
}

// 定义动态数组查找函数
void *Array_get(T array, int i)
{
    // 判断动态数组是否合法
    assert(array);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < array->length);

    // 计算得到地址
    return array->array + i * array->size;
}

// 定义动态数组添加函数
void *Array_put(T array, int i, void *elem)
{
    // 判断动态数组是否合法
    assert(array);
    // 判断索引值参数是否在范围内
    assert(i >= 0 && i < array->length);
    // 判断内容指针参数是否合法
    assert(elem);
    // 拷贝内容到指定位置
    memcpy(array->array + i * array->size, elem, array->size);

    // 返回内容指针
    return elem;
}

// 定义动态数组重置大小函数
void Array_resize(T array, int length)
{
    // 判断动态数组是否合法
    assert(array);
    // 判断所需长度是否合法
    assert(length >= 0);
    // 如果原长度为零，则直接分配所需的数组空间
    if(array->length == 0)
        array->array = ALLOC(length * array->size);
    // 若新长度不为零，则重新分配内存空间
    else if(length > 0)
        RESIZE(array->array, length * array->size);
    // 若新长度为零，则直接释放数组空间
    else
        FREE(array->array);
    // 更新动态数组长度
    array->length = length;
}

// 定义动态数组拷贝函数
T Array_copy(T array, int length)
{
    // 声明新动态数组结构体指针
    T copy;

    // 判断动态数组是否合法
    assert(array);
    // 判断所需长度是否合法
    assert(length >= 0);
    // 新建动态数组
    copy = Array_new(length, array->size);
    // 当新动态数组长度大于等于原动态数组且原动态数组长度大于零时，按原动态数组长度进行内存拷贝
    if(copy->length >= array->length && array->length > 0)
        memcpy(copy->array, array->array, array->length);
    // 当原动态数组长度大于新动态数组且新动态数组长度大于零时，按新动态数组长度进行内存拷贝
    else if(array->length > copy->length && copy->length > 0)
        memcpy(copy->array, array->array, copy->length);

    // 返回新动态数组
    return copy;
}

// 定义动态数组结构体初始化函数
// 仅能通过这个函数来初始化一个动态数组描述符的结构体，有助于减少耦合
// 只要这个函数原型不变化，即使用户需要在结构体中添加更多的成员
// 也会被封装在此函数中，不会波及到用户代码
void ArrayRep_init(T array, int length, int size, void *ary)
{
    // 判断动态数组是否合法
    assert(array);
    // 判断所需长度和分配空间是否匹配
    assert(ary && length > 0 || length == 0 && ary == NULL);
    // 判断成员大小是否合法
    assert(size > 0);
    // 更新动态数组成员的数量
    array->length = length;
    // 更新动态数组成员的大小
    array->size = size;
    // 判断成员数量是否为零，若为零则将数组参数指向空
    if(length > 0)
        array->array = ary;
    else
        array->array = NULL;
}

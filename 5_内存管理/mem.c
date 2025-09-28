// 调库
#include <stdlib.h>
#include <stddef.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

// 定义内存异常
const Except_T Mem_Failed = {"Allocation Failed"};

// 定义内存分配函数（不初始化）
void *Mem_alloc(long nbytes, const char *file, int line)
{
    // 声明内存指针
    void *ptr;

    // 判断内存分配大小是否合法
    assert(nbytes > 0);
    // 调用标准库内存分配函数
    ptr = malloc(nbytes);
    // 判断是否分配成功，若为否，则抛出异常
    if(ptr == NULL)
    {
        // 并判断文件指针时候为空，当前分支在抛出异常的位置不同
        if(file == NULL)
            RAISE(Mem_Failed);
        else
            Except_raise(&Mem_Failed, file, line);
    }

    // 返回内存指针
    return ptr;
}

// 定义内存分配函数（全初始化为0）
void *Mem_calloc(long count, long nbytes, const char *file, int line)
{
    // 声明内存指针
    void *ptr;

    // 判断内存分配数量和大小是否合法
    assert(count > 0);
    assert(nbytes > 0);
    // 调用标准库内存分配函数（这里会将count和nbytes相乘作为分配大小）
    ptr = calloc(count, nbytes);
    // 判断是否分配成功，若为否，则抛出异常
    if(ptr == NULL)
    {
        if(file == NULL)
            RAISE(Mem_Failed);
        else
            Except_raise(&Mem_Failed, file, line);
    }

    // 返回内存指针
    return ptr;
}

// 定义内存释放函数
void *Mem_free(void *ptr, const char *file, int line)
{
    // 判断内存指针时候为空，若不为空，则释放内存，若为空则跳过
    if(ptr)
        free(ptr);
}

// 定义内存重新分配函数
void *Mem_resize(void *ptr, long nbytes, const char *file, int line)
{
    // 判断内存指针和内存分配大小是否合法
    assert(ptr);
    assert(nbytes > 0);
    // 调用标准库内存重新分配函数
    ptr = realloc(ptr, nbytes);
    // 判断是否分配成功，若为否，则抛出异常
    if(ptr == NULL)
    {
        if(file == NULL)
            RAISE(Mem_Failed);
        else
            Except_raise(&Mem_Failed, file, line);
    }

    // 返回内存指针
    return ptr;
}

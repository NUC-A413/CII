#ifndef ARENA_INCLUDED
#define ARENA_INCLUDED

// 调用异常处理库
#include "except.h"

// 库内简化写法
#define T Arena_T

// 声明实存块结构体隐性指针
typedef struct T *T;
// TIP：个人不推荐这么写，但是许多项目都有这种写法
// 这样写法的区分主要是看声明定义的时候前面有没有struct关键字
// 不建议这样写的原因是因为实际操作的是一级指针变量
// 而再用其定义指针时，实际定义的是一个二级指针，但写出来看起来像一级指针
// 这样写极易造成误解，可读性较低

// 声明内存池新建异常
extern const Except_T Arena_NewFailed;
// 声明实存块异常
extern const Except_T Arena_Failed;

// 声明内存池新建函数
extern T Arena_new(void);
// 声明内存池销毁函数
extern void Arena_dispose(T *ap);
// 声明内存池内存分配函数（不初始化）
extern void *Arena_alloc(T arena, long nbytes, const char *file, int line);
// 声明内存池内存分配函数（全初始化为0）
extern void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line);
// 声明内存池内存释放函数
extern void Arena_free(T arena);

#undef T
#endif

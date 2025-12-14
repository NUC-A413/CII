// 调库
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "arena.h"

#define T Arena_T

// 定义内存池新建异常
const Except_T Arena_NewFailed = {"Arena Creation Failed"};
// 定义实存块异常
const Except_T Arena_Failed = {"Arena Allocation Failed"};

// 定义空闲实存块数量限额
#define THRESHOLD 10

// 定义实存块表头结构体
struct T
{
    // 该参数指向前置存储块
    T prev;
    // 该参数指向前置存储块的第一个空闲位置
    char *avail;
    // 该参数指向紧靠前置存储块的末尾的下一内存单元
    char *limit;
};

// 定义大小约束共用体，用来约束分配内存大小和进行判断
union align
{
    int i;
    long l;
    long *lp;
    void *p;
    void (*fp)(void);
    float f;
    double d;
    long double ld;
};

// 定义实存块表头共用体（是为了约束实存块表头结构体的大小）
union header
{
    struct T b;
    union align a;
};

// 声明静态空闲实存块链表
static T freechunks;
// 声明静态空闲实存块数量
static int nfree;

// 定义内存池新建函数
T Arena_new(void)
{
    // 定义内存池指针并分配其实存块表头的内存空间
    T arena = malloc(sizeof(*arena));

    // 若分配失败，则抛出异常
    if(arena == NULL)
        RAISE(Arena_NewFailed);
    // 将内存池指针里的参数均设定为空值
    arena->prev = NULL;
    arena->limit = arena->avail = NULL;

    // 返回内存池指针
    return arena;
}

// 定义内存池销毁函数
void Arena_dispose(T *ap)
{
    // 判断内存池指针和二级指针是否合法
    assert(ap && *ap);
    // 释放内存池中的内存
    Arena_free(*ap);
    // 释放内存池指针所占用的内存
    free(*ap);
    // 更新内存池指针为空值
    *ap = NULL;
}

// 定义内存池内存分配函数（不初始化）
void *Arena_alloc(T arena, long nbytes, const char *file, int line)
{
    // 判断内存池指针和内存分配大小是否合法
    assert(arena);
    assert(nbytes > 0);
    // 将内存分配大小向上取整为大小约束共用体的整数倍
    nbytes = ((nbytes + sizeof(union align) - 1) / (sizeof(union align))) * (sizeof(union align));
    // 循环判断当前实存块是否有足够大的内存空间
    while(nbytes > arena->limit - arena->avail)
    {
        // 声明新实存块指针
        T ptr;
        // 声明实存块限制指针
        char *limit;
        // 查看是否有空闲的实存块
        if((ptr = freechunks) != NULL)
        {
            // 空闲实存块链表弹栈
            freechunks = freechunks->prev;
            // 空闲实存块数量减少
            nfree--;
            // 更新限制指针
            limit = ptr->limit;
        }
        else
        {
            // 定义所需分配内存大小，这里由实存块头加实际需求量加冗余量计算得出
            long m = sizeof(union header) + nbytes + 10 * 1024;
            // 分配内存空间
            ptr = malloc(m);
            // 判断是否分配成功，若为否，则抛出异常
            if(ptr == NULL)
            {
                // 并判断文件指针时候为空，当前分支在抛出异常的位置不同
                if(file == NULL)
                    RAISE(Arena_Failed);
                else
                    Except_raise(&Arena_Failed, file, line);
            }
            // 更新限制指针
            limit = (char *)ptr + m;
        }
        // 拷贝内存池内容
        *ptr = *arena;
        // 内存池中添加新实存块
        arena->avail = (char *)((union header *)ptr + 1);
        arena->limit = limit;
        arena->prev = ptr;
    }
    // 实存块中的空闲指针下移，给出所需内存空间
    arena->avail += nbytes;

    // 返回刚空闲指针下移腾出的空间首地址
    return arena->avail - nbytes;
}

// 定义内存池内存分配函数（全初始化为0）
void *Arena_calloc(T arena, long count, long nbytes, const char *file, int line)
{
    // 声明内存指针
    void *ptr;

    // 判断内存分配数量是否合法
    assert(count > 0);
    // 分配内存空间
    ptr = Arena_alloc(arena, count * nbytes, file, line);
    // 初始化内存空间内为全0
    memset(ptr, "\0", count * nbytes);

    // 返回内存指针
    return ptr;
}

// 定义内存池内存释放函数
void Arena_free(T arena)
{
    // 判断内存池指针是否合法
    assert(arena);
    // 判断是否还有实存块
    while(arena->prev)
    {
        // 存储最后一个实存块的表头
        struct T tmp = *arena->prev;
        // 判断空闲实存块数量是否超额
        if(nfree < THRESHOLD)
        {
            // 将最后一个实存块加入空闲实存块链表
            arena->prev->prev = freechunks;
            freechunks = arena->prev;
            // 空闲实存块数量增加
            nfree++;
            // 将最后一个实存块的限制指针更新为自身实存块限制地址
            freechunks->limit = arena->limit;
        }
        // 空闲实存块数量超额则直接释放最后一个实存块
        else
            free(arena->prev);
        // 将内存池内容更新为最后一个实存块表头
        *arena = tmp;
        // 遍历去释放前面所有实存块
    }
    // 直到内存池清空
    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}

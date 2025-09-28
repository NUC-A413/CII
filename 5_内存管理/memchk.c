// 调库
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

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
}

// 定义哈希算法计算方式
#define hash(p, t) (((unsigned long)(p)>>3) & (sizeof (t)/sizeof ((t)[0])-1))
// 定义新分配的空闲描述符数量
#define NDESCRIPTORS 512
// 定义新内存空间固定新增值
#define NALLOC 4096

// 定义内存异常
const Except_T Mem_Failed = {"Allocation Failed"};

// 定义描述符结构体
static struct descriptor
{
    // 该参数表明是否为可分配的空闲内存
    struct descriptor *free;
    // 该参数指向该链表的下一节点
    struct descriptor *link;
    // 该参数指向实际分配内存空间
    const void *ptr;
    // 该参数指定内存分配的大小
    long size;
    // 该参数指定内存分配函数使用的文件
    const char *file;
    // 该参数指定内存分配函数使用的行数
    int line;
} *htab[2048]; // 声明描述符散列

// 定义空闲内存链表
static struct descriptor freelist = {&freelist}

// 定义内部内存分配查找函数
static struct descriptor *find(const void *ptr)
{
    // 定义描述符指针，使用哈希算法计算
    struct descriptor *bp = htab[hash(ptr, htab)];

    // 当该描述符指针存在且指向的内存空间不同于传入参数时，沿链表向下查找，直到相同或下一节点为空
    while(bp && bp->ptr != ptr)
        bp = bp->link;

    // 回传查找到的描述符指针或空值
    return bp;
}

// 定义内部描述符块分配函数
static struct descriptor *dalloc(void *ptr, long size, const char *file, int line)
{
    // 定义静态描述符指针，确保离开该函数时依然有效
    static struct descriptor *avail;
    // 声明静态空闲描述符数量变量，确保离开该函数时依然有效
    static int nleft;

    // 判断上次分配的空闲描述符是否使用完，若未使用完则不分配
    if(nleft <= 0)
    {
        // 分配描述符内存空间
        avail = malloc(NDESCRIPTORS*sizeof(*avail));
        // 判断是否分配失败，失败则返回空值，便于后续异常抛出
        if(avail == NULL)
            return NULL;
        // 更新空闲描述符数量
        nleft = NDESCRIPTORS;
    }
    // 更新刚分配的描述符内容
    avail->ptr = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    avail->free = avail->link = NULL;
    // 空闲描述符数量减一
    nleft--;

    // 返回下一空闲描述符地址
    return avail++;
}

// 定义内存分配函数（不初始化）
void *Mem_alloc(long nbytes, const char *file, int line)
{
    // 声明描述符指针
    struct descriptor *bp;
    // 声明内存指针
    void *ptr;

    // 判断内存分配大小是否合法
    assert(nbytes > 0);
    // 将内存分配大小向上取整为大小约束共用体的整数倍
    nbytes = ((nbytes + sizeof (union align) - 1)/(sizeof (union align)))*(sizeof (union align));
    // 循环判断是否有符合条件的空闲内存空间
    for(bp = freelist.free; bp; bp = bp->free)
    {
        // 判断空闲内存空间是否大于所需大小
        if(bp->size > nbytes)
        {
            // 更新空闲内存空间大小
            bp->size -= nbytes;
            // 更新内存指针（将符合需求的内存空间分配给内存指针）
            ptr = (char *)bp->ptr + bp->size;
            // 更新描述符指针内容为刚分配内存空间，并判断是否更新成功
            if((bp = dalloc(ptr, nbytes, file, line)) != NULL)
            {
                // 计算散列位置
                unsigned h = hash(ptr, htab);
                // 将描述符指针挂载到散列
                bp->link = htab[h];
                htab[h] = bp;
                // 返回内存指针
                return ptr;
                // 在看到这里的时候，以为作者忘记将当前空闲内存节点剔出空闲内存链表了
                // 后来发现，仅仅是从空闲的内存空间取了所需的空间，实际空闲内存链表并不需要改动
                // 这也就是为什么在上面判断空闲内存空间是否大于所需大小，而不是大于等于
            }
            // 若更新描述符指针失败，则抛出异常
            else
            {
                // 并判断文件指针时候为空，当前分支在抛出异常的位置不同
                if(file == NULL)
                    RAISE(Mem_Failed);
                else
                    Except_raise(&Mem_Failed, file, line);
            }
        }
        // 若当前空闲内存链表中没有符合条件的内存空间，则需要在空闲内存列表加入新的内存空间
        if(bp == &freelist)
        {
            // 声明新描述符指针
            struct descriptor *newptr;
            // 分配新内存空间并分配新描述符空间，若失败，则抛出异常
            // 这里新内存空间的大小是所需大小加上一个固定值
            if((ptr = malloc(nbytes + NALLOC)) == NULL || (newptr = dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL)
            {
                if(file == NULL)
                    RAISE(Mem_Failed);
                else
                    Except_raise(&Mem_Failed, file, line);
            }
            // 将新分配的内存空间加入到空闲内存链表
            newptr->free = freelist.free;
            freelist.free = newptr;
            // 在这里重新分配过后，在下一次遍历空闲内存链表时一定存在所需空闲内存空间
        }
    }

    // 若还是运行至这里，则抛出异常，并返回空值
    assert(0);
    return NULL;
}

// 定义内存分配函数（全初始化为0）
void *Mem_calloc(long count, long nbytes, const char *file, int line)
{
    // 声明内存指针
    void *ptr;

    // 判断内存分配数量和大小是否合法
    assert(count > 0);
    assert(nbytes > 0);
    // 分配内存空间
    ptr = Mem_alloc(count*nbytes, file, line);
    // 初始化内存空间内为全0
    memset(ptr, "\0", count*nbytes);

    // 返回内存指针
    return ptr;
}

// 定义内存释放函数
void Mem_free(void *ptr, const char *file, int line)
{
    // 判断内存指针时候为空，若不为空，则释放内存，若为空则跳过
    if(ptr)
    {
        // 声明描述符指针
        struct descriptor *bp;
        // 判断内存空间是否符合约束，判断内存空间是否在内存分配散列，判断内存空间是否空闲，若存在是，则抛出异常
        if(((unsigned long)ptr)%(sizeof (union align)) != 0 || (bp = find(ptr)) == NULL || bp->free)
            Except_raise(&Assert_Failed, file, line);
        // 添加该内存空间至空闲内存链表
        bp->free = freelist.free;
        freelist.free = bp;
    }
}

// 定义内存重新分配函数
void *Mem_resize(void *ptr, long nbytes, const char *file, int line)
{
    // 声明描述符指针
    struct descriptor *bp;
    // 声明新内存指针
    void *newptr;

    // 判断内存指针和内存分配大小是否合法
    assert(ptr);
    assert(nbytes > 0);
    // 判断内存空间是否符合约束，判断内存空间是否在内存分配散列，判断内存空间是否空闲，若存在是，则抛出异常
    if(((unsigned long)ptr)%(sizeof (union align)) != 0 || (bp = find(ptr)) == NULL || bp->free)
        Except_raise(&Assert_Failed, file, line);
    // 分配新的内存空间
    newptr = Mem_alloc(nbytes, file, line);
    // 拷贝原内存空间内容至新内存空间
    memcpy(newptr, ptr, nbytes < bp->size ? nbytes :bp->size);
    // 释放原内存空间
    Mem_free(ptr, file, line);

    // 返回新内存指针
    return newptr;
}

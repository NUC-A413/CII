// 调库
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#include "bit.h"
#include "mem.h"

#define T Bit_T

// 定义位向量结构体
struct T
{
    // 该参数为位向量总长度
    int length;
    // 该参数为指向位向量内存的字节指针
    unsigned char *bytes;
    // 该参数为指向位向量内存的字指针
    unsigned long *words;
};

// 字长位数
#define BPW (8*sizeof (unsigned long))
// 定义长度转字节数宏函数
#define nbytes(len) ((((len) + 8 - 1)&(~(8-1)))/8)
// 定义长度转字数宏函数
#define nwords(len) ((((len) + BPW - 1)&(~(BPW-1)))/BPW)
// 定义集合操作宏函数，这里用宏函数是为了调用两个位向量变量和将位操作符号作为参数传进来
#define setop(sequal, snull, tnull, op) \
    if(s == t){assert(s); return sequal; } /* 若两个位向量相同，就返回相同参数 */ \
    else if(s == NULL){assert(t); return snull; } /* 若位向量s为空，则返回s空参数 */ \
    else if(t == NULL) return tnull; /* 若位向量t为空，则返回t空参数 */ \
    else{ \
        int i; T set; /* 声明计数变量和位向量结构体指针 */ \
        assert(s->length == t->length); /* 判断两个位向量长度是否相同 */ \
        set = Bit_new(s->length); /* 初始化位向量 */ \
        for(i = nwords(s->length); --i >= 0; ) /* 遍历两个位向量，以字为单位 */ \
            set->words[i] = s->words[i] op t->words[i]; /* 按传入的符号对两个位向量进行运算，传入新位向量中 */ \
        return set; /* 返回新位向量 */ \
    }

// 高有效位掩码
unsigned char msbmask[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80};
// 低有效位掩码
unsigned char lsbmask[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

// 定义静态位向量拷贝函数
static T copy(T t)
{
    // 声明位向量结构体指针
    T set;

    // 判断位向量是否合法
    assert(t);
    // 初始化位向量
    set = Bit_new(t->length);
    // 遍历整个位向量
    if(t->length > 0)
        // 拷贝所有内容，以字节为单位
        memcpy(set->bytes, t->bytes, nbytes(t->length));

    // 返回新位向量
    return set;
}

// 定义位向量创建函数
T Bit_new(int length)
{
    // 声明位向量结构体指针
    T set;

    // 判断长度是否合法
    assert(length >= 0);
    // 分配内存空间
    NEW(set);
    // 判断长度是否为零，若为否，则分配位向量内存空间
    if(length > 0)
        set->words = CALLOC(nwords(length), sizeof(unsigned long));
    else
        set->words = NULL;
    // 将字指针强制类型转换后传入字节指针
    set->bytes = (unsigned char *)set->words;
    // 更新位向量长度
    set->length = length;

    // 返回位向量
    return set;
}

// 定义位向量释放函数
void Bit_free(T *set)
{
    // 判断位向量指针和位向量是否合法
    assert(set && *set);
    // 释放位向量内容
    FREE((*set)->words);
    // 释放位向量头
    FREE(*set);
}

// 定义位向量计长函数
int Bit_length(T set)
{
    // 判断位向量是否合法
    assert(set);

    // 返回位向量总长度
    return set->length;
}

// 定义位向量计数函数
int Bit_count(T set)
{
    // 定义计数变量
    int length = 0, n;
    // 该静态数组中存储了从0至F间二进制时所含有的1的数量
    static char count[] =
    {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    };

    // 判断位向量是否合法
    assert(set);
    // 遍历位向量中的所有字节
    for(n = nbytes(set->length); --n >= 0; )
    {
        // 存储当前字节
        unsigned char c = set->bytes[n];
        // 分别计算后四位和前四位中1的数量
        length += count[c & 0xF] + count[c >> 4];
    }

    // 返回位向量数量
    return length;
}

// 定义位向量查找函数
int Bit_get(T set, int n)
{
    // 判断位向量是否合法
    assert(set);
    // 判断索引值参数是否在范围内
    assert(0 <= n && n < set->length);

    // 返回该索引位置内容
    return ((set->bytes[n / 8] >> (n % 8)) & 1);
}

// 定义位向量添加函数
int Bit_put(T set, int n, int bit)
{
    // 声明内容变量
    int prev;

    // 判断位向量是否合法
    assert(set);
    // 判断修改为的内容是否合法
    assert(bit == 0 || bit == 1);
    // 判断索引值参数是否在范围内
    assert(0 <= n && n < set->length);
    // 记录前置内容
    prev = ((set->bytes[n / 8] >> (n % 8)) & 1);
    // 更新该位置内容
    if(bit == 1)
        set->bytes[n / 8] |= 1 << (n % 8);
    else
        set->bytes[n / 8] &= ~(1 << (n % 8));

    // 返回前置内容
    return prev;
}

// 定义位向量批量清除函数
void Bit_clear(T set, int lo, int hi)
{
    // 判断位向量是否合法
    assert(set);
    // 判断索引值参数是否在范围内
    assert(0 <= lo && hi < set->length);
    // 判断索引值参数是否相对合法
    assert(lo <= hi);
    // 判断是否需要跨字节修改
    if(lo / 8 < hi / 8)
    {
        // 声明计数变量
        int i;
        // 更新低位不完整字节内容
        set->bytes[lo / 8] &= ~msbmask[lo % 8];
        // 更新中间连续完整字节内容
        for(i = lo / 8 + 1; i < hi / 8; i++)
            set->bytes[i] = 0;
        // 更新高位不完整字节内容
        set->bytes[hi / 8] &= ~lsbmask[hi % 8];
    }
    // 若不需要跨字节修改（利用掩码相与得到修改范围）
    else
        set->bytes[lo / 8] &= ~(msbmask[lo % 8] & lsbmask[hi % 8]);
}

// 定义位向量批量设置函数
void Bit_set(T set, int lo, int hi)
{
    // 判断位向量是否合法
    assert(set);
    // 判断索引值参数是否在范围内
    assert(0 <= lo && hi < set->length);
    // 判断索引值参数是否相对合法
    assert(lo <= hi);
    // 判断是否需要跨字节修改
    if(lo / 8 < hi / 8)
    {
        // 更新低位不完整字节内容
        set->bytes[lo / 8] |= msbmask[lo % 8];
        // 更新中间连续完整字节内容
        {
            int i;
            for(i = lo / 8 + 1; i < hi / 8; i++)
                set->bytes[i] = 0xFF;
        }
        // 更新高位不完整字节内容
        set->bytes[hi / 8] |= lsbmask[hi % 8];
    }
    // 若不需要跨字节修改（利用掩码相与得到修改范围）
    else
        set->bytes[lo / 8] |= (msbmask[lo % 8] & lsbmask[hi % 8]);
}

// 定义位向量翻转函数
void Bit_not(T set, int lo, int hi)
{
    // 判断位向量是否合法
    assert(set);
    // 判断索引值参数是否在范围内
    assert(0 <= lo && hi < set->length);
    // 判断索引值参数是否相对合法
    assert(lo <= hi);
    // 判断是否需要跨字节修改
    if(lo / 8 < hi / 8)
    {
        // 声明计数变量
        int i;
        // 更新低位不完整字节内容
        set->bytes[lo / 8] ^= msbmask[lo % 8];
        // 更新中间连续完整字节内容
        for(i = lo / 8 + 1; i < hi / 8; i++)
            set->bytes[i] ^= 0xFF;
        // 更新高位不完整字节内容
        set->bytes[hi / 8] ^= lsbmask[hi % 8];
    }
    // 若不需要跨字节修改（利用掩码相与得到修改范围）
    else
        set->bytes[lo / 8] ^= (msbmask[lo % 8] & lsbmask[hi % 8]);
}

// 定义位向量子集函数
int Bit_leq(T s, T t)
{
    // 声明计数变量
    int i;

    // 判断位向量是否合法
    assert(s && t);
    // 判断两个位向量长度是否相同
    assert(s->length == t->length);
    // 遍历整个位向量，以字为单位
    for(i = nwords(s->length); --i >= 0; )
        // 若位向量s中存在位向量t中不存在返回否
        if((s->words[i] & ~t->words[i]) != 0)
            return 0;

    // 返回真
    return 1;
}

// 定义位向量真子集函数
int Bit_lt(T s, T t)
{
    // 声明计数变量和定义真子集标志
    int i, lt = 0;

    // 判断位向量是否合法
    assert(s && t);
    // 判断两个位向量长度是否相同
    assert(s->length == t->length);
    // 遍历整个位向量，以字为单位
    for(i = nwords(s->length); --i >= 0; )
    {
        // 若位向量s中存在位向量t中不存在返回否
        if((s->words[i] & ~t->words[i]) != 0)
            return 0;
        // 若位向量s中不存在位向量t中不存在的，且两个位向量不完全相同，则更新真子集标志
        else if(s->words[i] != t->words[i])
            lt |= 1;
    }

    // 返回真子集标志
    return lt;
}

// 定义位向量相等函数
int Bit_eq(T s, T t)
{
    // 声明计数变量
    int i;

    // 判断位向量是否合法
    assert(s && t);
    // 判断两个位向量长度是否相同
    assert(s->length == t->length);
    // 遍历整个位向量，以字为单位
    for(i = nwords(s->length); --i >= 0; )
        // 若两个位向量不完全相同返回否
        if(s->words[i] != t->words[i])
            return 0;

    // 返回真
    return 1;
}

// 定义位向量遍历调用函数
void Bit_map(T set, void apply(int n, int bit, void *cl), void *cl)
{
    // 声明计数变量
    int n;

    // 判断位向量是否合法
    assert(set);
    // 遍历整个位向量
    for(n = 0; n < set->length; n++)
        // 将各位内容作为参数调用回调函数
        apply(n, ((set->bytes[n / 8] >> (n % 8)) & 1), cl);
}

// 定义位向量并集函数
T Bit_union(T s, T t)
{
    // 相同返回位向量t的拷贝
    // 位向量s为空则返回位向量t的拷贝
    // 位向量t为空则返回位向量s的拷贝
    // 符号是位或运算符
    setop(copy(t), copy(t), copy(s), | )
}

// 定义位向量交集函数
T Bit_inter(T s, T t)
{
    // 相同返回位向量t的拷贝
    // 位向量s为空则返回位向量t大小的空位向量
    // 位向量t为空则返回位向量s大小的空位向量
    // 符号是位与运算符
    setop(copy(t), Bit_new(t->length), Bit_new(s->length), &)
}

// 定义位向量差集函数
T Bit_minus(T s, T t)
{
    // 相同返回位向量s大小的空位向量
    // 位向量s为空则返回位向量t大小的空位向量
    // 位向量t为空则返回位向量s大小的空位向量
    // 符号是先取反后位与
    setop(Bit_new(s->length), Bit_new(t->length), copy(s), &~)
}

// 定义位向量对称差集函数
T Bit_diff(T s, T t)
{
    // 相同返回位向量s大小的空位向量
    // 位向量s为空则返回位向量t的拷贝
    // 位向量t为空则返回位向量s的拷贝
    // 符号是位异或运算符
    setop(Bit_new(s->length), copy(t), copy(s), ^)
}

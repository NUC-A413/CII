// 调库
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "ap.h"
#include "fmt.h"
#include "xp.h"
#include "mem.h"

#define T AP_T

// 定义任意精度数据结构体
struct T
{
    // 该参数为正负符号
    int sign;
    // 该参数为使用中的位数
    int ndigits;
    // 该参数为空间总位数
    int size;
    // 该参数指向扩展精度数据
    XP_T digits;
};

// 定义判断为零宏函数
#define iszero(x) ((x)->ndigits==1 && (x)->digits[0]==0)
// 定义最大使用位数宏函数
#define maxdigits(x,y) ((x)->ndigits > (y)->ndigits ? (x)->ndigits : (y)->ndigits)
// 定义判断为一宏函数
#define isone(x) ((x)->ndigits==1 && (x)->digits[0]==1)

// 声明静态任意精度归一化函数
static T normalize(T z, int n);
// 声明静态任意精度绝对值比较函数
static int cmp(T x, T y);

// 定义静态任意精度数据构建函数
static T mk(int size)
{
    // 分配任意精度数据结构体内存空间
    T z = CALLOC(1, sizeof(*z) + size);

    // 判断位数是否合法
    assert(size > 0);
    // 更新符号为正
    z->sign = 1;
    // 更新总位数
    z->size = size;
    // 更新使用位数
    z->ndigits = 1;
    // 更新扩展精度数据指针
    z->digits = (XP_T)(z + 1);

    // 返回结构体指针
    return z;
}

// 定义静态任意精度设置函数
static T set(T z, long int n)
{
    // 当原数为最小负数时，取最大正数加一
    if(n == LONG_MIN)
        XP_fromint(z->size, z->digits, LONG_MAX + 1UL);
    // 当原数为负数时，取相反数
    else if(n < 0)
        XP_fromint(z->size, z->digits, -n);
    // 当原数为正数时，取本身
    else
        XP_fromint(z->size, z->digits, n);
    // 根据原数更新符号位
    z->sign = n < 0 ? -1 : 1;

    // 返回调用静态任意精度归一化函数
    return normalize(z, z->size);
}

// 定义静态任意精度归一化函数
static T normalize(T z, int n)
{
    // 更新使用位数
    z->ndigits = XP_length(n, z->digits);

    // 返回结构体指针
    return z;
}

// 定义静态任意精度绝对值加法函数
static T add(T z, T x, T y)
{
    // 取其中一个加数的使用位数做基准
    int n = y->ndigits;

    // 将另一个加数的使用位数与基准做对比，若小于，则交换两数
    if(x->ndigits < n)
        return add(z, y, x);
    // 若另一个加数的使用位数大于基准
    else if(x->ndigits > n)
    {
        // 定义进位项，调用扩展精度加法函数
        int carry = XP_add(n, z->digits, x->digits, y->digits, 0);
        // 调用扩展精度一位加法函数，求更高位数值
        z->digits[z->size - 1] = XP_sum(x->ndigits - n, &z->digits[n], &x->digits[n], carry);
    }
    // 若两个加数使用位数相同
    else
        // 调用扩展精度加法函数
        z->digits[n] = XP_add(n, z->digits, x->digits, y->digits, 0);

    // 返回调用静态任意函数归一化函数更新使用位数
    return normalize(z, z->size);
}

// 定义静态任意精度绝对值减法函数
static T sub(T z, T x, T y)
{
    // 声明借位项，取减数的使用位数做基准
    int borrow, n = y->ndigits;

    // 调用扩展精度减法函数
    borrow = XP_sub(n, z->digits, x->digits, y->digits, 0);
    // 若被减数的使用位数大于基准
    if(x->ndigits > n)
        // 调用扩展精度一位减法函数求更高位数值
        borrow = XP_diff(x->ndigits - n, &z->digits[n], &x->digits[n], borrow);
    // 判断借位是否归零
    assert(borrow == 0);

    // 返回调用静态任意函数归一化函数更新使用位数
    return normalize(z, z->size);
}

// 定义静态任意精度乘法取模函数
static T mulmod(T x, T y, T p)
{
    // 声明新数据结构体指针并定义乘积数据结构体指针，调用任意精度乘法函数解算乘积项
    T z, xy = AP_mul(x, y);

    // 调用任意精度取模函数
    z = AP_mod(xy, p);
    // 释放乘积项
    AP_free(&xy);

    // 返回新数据
    return z;
}

// 定义静态任意精度绝对值比较函数
static int cmp(T x, T y)
{
    // 若使用位数不同，则返回使用位数差值
    if(x->ndigits != y->ndigits)
        return x->ndigits - y->ndigits;
    // 若使用位数相同
    else
        // 返回调用扩展精度比较函数
        return XP_cmp(x->ndigits, x->digits, y->digits);
}

// 定义任意精度新建函数
T AP_new(long int n)
{
    // 返回新建好的任意精度数据
    return set(mk(sizeof(long int)), n);
}

// 定义任意精度字符串获取函数
T AP_fromstr(const char *str, int base, char **end)
{
    // 声明新数据结构体指针
    T z;
    // 定义字符指针记录字符串首字符地址
    const char *p = str;
    // 声明末尾字符指针并定义符号字符
    char *endp, sign = '\0';
    // 声明进位项
    int carry;

    // 判断上面字符串指针常量是否合法
    assert(p);
    // 判断底数是否超出范围
    assert(base >= 2 && base <= 36);
    // 忽略字符串最前面的空格
    while(*p && isspace(*p))
        p++;
    // 提取符号位
    if(*p == '-' || *p == '+')
        sign = *p++;
    {
        // 声明起始字符指针
        const char *start;
        // 声明定义计数变量
        int k, n = 0;

        // 忽略数字前面的零
        for(; *p == '0' && p[1] == '0'; p++);
        // 更新起始字符指针
        start = p;
        // 获取数字位数
        for(; ('0' <= *p && *p <= '9' && *p < '0' + base || 'a' <= *p && *p <= 'z' && *p < 'a' + base - 10 || 'A' <= *p && *p <= 'Z' && *p < 'A' + base - 10); p++)
            n++;
        // 计算底数转为二进制最少需要的位数
        for(k = 1; (1 << k) < base; k++);
        // 对新数据分配内存空间
        z = mk(((k * n + 7) & ~7) / 8);
        // 重置字符指针至数字起始位
        p = start;
    }
    // 调用扩展精度字符串获取函数
    carry = XP_fromstr(z->size, z->digits, p, base, &endp);
    // 判断是否提取成功
    assert(carry == 0);
    // 调用静态任意函数归一化函数更新使用位数
    normalize(z, z->size);
    // 若数字部分无法提取
    if(endp == p)
    {
        // 保存完整字符串
        endp = (char *)str;
        // 新数据归零
        z = AP_new(0);
    }
    else
        // 更新符号位
        z->sign = iszero(z) || sign != '-' ? 1 : -1;
    // 判断结束字符串指针是否存在，保存后续字符串
    if(end)
        *end = (char *)endp;

    // 返回新数据
    return z;
}

// 定义任意精度整型表示函数
long int AP_toint(T x)
{
    // 声明存储变量
    unsigned long u;

    // 判断原数据是否合法
    assert(x);
    // 调用扩展精度整型表示函数
    u = XP_toint(x->ndigits, x->digits) % (LONG_MAX + 1UL);
    // 引入符号
    if(x->sign == -1)
        return -(long)u;
    else
        return (long)u;
}

// 定义任意精度字符串表示函数
char *AP_tostr(char *str, int size, int base, T x)
{
    // 声明扩展精度数据
    XP_T q;

    // 判断原数据是否合法
    assert(x);
    // 判断底数是否超出范围
    assert(base >= 2 && base <= 36);
    // 判断字符串和字符串长度是否合法
    assert(str == NULL || size > 1);
    // 若字符串为空
    if(str == NULL)
    {
        {
            // 声明计数变量
            int k;

            // 估算数字位数转字符位数的系数
            for(k = 5; (1 << k) > base; k--);
            // 计算字符串位数，向上取整并加上结束字符空间
            size = (8 * x->ndigits) / k + 1 + 1;
            // 当原数为负数时，字符串位数再加一
            if(x->sign == -1)
                size++;
        }
        // 分配字符串内存空间
        str = ALLOC(size);
    }
    // 对扩展精度数据分配内存
    q = ALLOC(x->ndigits);
    // 从任意精度向扩展精度拷贝数据
    memcpy(q, x->digits, x->ndigits);
    // 当原数为负数时，起始设置负号
    if(x->sign == -1)
    {
        str[0] = '-';
        // 调用扩展精度字符串表示函数
        XP_tostr(str + 1, size - 1, base, x->ndigits, q);
    }
    else
        // 调用扩展精度字符串表示函数
        XP_tostr(str, size, base, x->ndigits, q);
    // 释放扩展精度数据
    FREE(q);

    // 返回字符串
    return str;
}

// 定义任意精度格式化函数
void AP_fmt(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 声明新数据结构体指针
    T x;
    // 声明字符串
    char *buf;

    // 判断可变参数列表和格式化标志是否合法
    assert(app && flags);
    // 获取下一个任意精度型量
    x = va_arg(*app, T);
    // 判断原数据是否合法
    assert(x);
    // 调用任意精度字符串表示函数，转为字符串
    buf = AP_tostr(NULL, 0, 10, x);
    // 调用格式化字符串传递函数输出任意精度数据
    Fmt_putd(buf, strlen(buf), put, cl, flags, width, precision);
    // 释放字符串
    FREE(buf);
}

// 定义任意精度释放函数
void AP_free(T *z)
{
    // 判断结构体二级指针与结构体指针是否合法
    assert(z && *z);
    // 释放内存
    FREE(*z);
}

// 定义任意精度取负函数
T AP_neg(T x)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    // 调用静态构建函数分配内存空间
    z = mk(x->ndigits);
    // 拷贝使用的字节
    memcpy(z->digits, x->digits, x->ndigits);
    // 更新新数据的使用位数
    z->ndigits = x->ndigits;
    // 更新符号，对原数据符号取反
    z->sign = iszero(z) ? 1 : -x->sign;

    // 返回新数据
    return z;
}

// 定义任意精度加法函数
T AP_add(T x, T y)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 当两个加数符号相同时
    if(((x->sign ^ y->sign) == 0))
    {
        // 将两个加数的绝对值求和，对结果分配加数使用位数加一的内存空间
        z = add(mk(maxdigits(x, y) + 1), x, y);
        // 更新符号为一个加数的符号
        z->sign = iszero(z) ? 1 : x->sign;
    }
    // 若符号不同，比较两个加数绝对值大小，符号取绝对值大者的
    else if(cmp(x, y) > 0)
    {
        // 将两个数的绝对值求差，对结果分配绝对值大者使用位数的内存空间
        z = sub(mk(x->ndigits), x, y);
        // 更新符号位
        z->sign = iszero(z) ? 1 : x->sign;
    }
    else
    {
        // 将两个数的绝对值求差，对结果分配绝对值大者使用位数的内存空间
        z = sub(mk(y->ndigits), y, x);
        // 更新符号位
        z->sign = iszero(z) ? 1 : -x->sign;
    }

    // 返回新数据
    return z;
}

// 定义任意精度减法函数
T AP_sub(T x, T y)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 当两个数符号不同时
    if(!((x->sign ^ y->sign) == 0))
    {
        // 将两个数的绝对值求和，对结果分配两数使用位数加一的内存空间
        z = add(mk(maxdigits(x, y) + 1), x, y);
        // 更新符号为被减数的符号
        z->sign = iszero(z) ? 1 : x->sign;
    }
    // 若符号不同，比较两个加数绝对值大小，符号取绝对值大者的
    else if(cmp(x, y) > 0)
    {
        // 将两个数的绝对值求差，对结果分配绝对值大者使用位数的内存空间
        z = sub(mk(x->ndigits), x, y);
        // 更新符号位
        z->sign = iszero(z) ? 1 : x->sign;
    }
    else
    {
        // 将两个数的绝对值求差，对结果分配绝对值大者使用位数的内存空间
        z = sub(mk(y->ndigits), y, x);
        // 更新符号位
        z->sign = iszero(z) ? 1 : -x->sign;
    }

    // 返回新数据
    return z;
}

// 定义任意精度乘法函数
T AP_mul(T x, T y)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 调用静态构建函数分配内存空间
    z = mk(x->ndigits + y->ndigits);
    // 调用扩展精度乘法函数
    XP_mul(z->digits, x->ndigits, x->digits, y->ndigits, y->digits);
    // 调用静态任意函数归一化函数更新使用位数
    normalize(z, z->size);
    // 更新符号，当新数据为零或两个乘数符号相同时取正号
    z->sign = iszero(z) || ((x->sign ^ y->sign) == 0) ? 1 : -1;

    // 返回新数据
    return z;
}

// 定义任意精度除法函数
T AP_div(T x, T y)
{
    // 声明商和余数数据结构体指针
    T q, r;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 判断除数是否为零
    assert(!iszero(y));
    // 给商和余数分配内存空间
    q = mk(x->ndigits);
    r = mk(y->ndigits);
    {
        // 定义临时扩展精度数据，并分配内存
        XP_T tmp = ALLOC(x->ndigits + y->ndigits + 2);

        // 调用扩展精度除法函数
        XP_div(x->ndigits, q->digits, x->digits, y->ndigits, y->digits, r->digits, tmp);
        // 释放临时数据
        FREE(tmp);
    }
    // 调用静态任意函数归一化函数更新使用位数
    normalize(q, q->size);
    normalize(r, r->size);
    // 更新符号，当新数据为零或被除数与除数符号相同时取正号
    q->sign = iszero(q) || ((x->sign ^ y->sign) == 0) ? 1 : -1;
    // 当被除数和除数符号不同且余数不为零时
    if(!((x->sign ^ y->sign) == 0) && !iszero(r))
    {
        // 定义进位项，调用扩展精度一位加法函数
        int carry = XP_sum(q->size, q->digits, q->digits, 1);

        // 判断进位项是否为零
        assert(carry == 0);
        // 调用静态任意函数归一化函数更新使用位数
        normalize(q, q->size);
    }
    // 调用任意精度释放函数，释放余数数据
    AP_free(&r);

    // 返回商数据
    return q;
}

// 定义任意精度取模函数
T AP_mod(T x, T y)
{
    // 声明商和余数数据结构体指针
    T q, r;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 判断除数是否为零
    assert(!iszero(y));
    // 给商和余数分配内存空间
    q = mk(x->ndigits);
    r = mk(y->ndigits);
    {
        // 定义临时扩展精度数据，并分配内存
        XP_T tmp = ALLOC(x->ndigits + y->ndigits + 2);

        // 调用扩展精度除法函数
        XP_div(x->ndigits, q->digits, x->digits, y->ndigits, y->digits, r->digits, tmp);
        // 释放临时数据
        FREE(tmp);
    }
    // 调用静态任意函数归一化函数更新使用位数
    normalize(q, q->size);
    normalize(r, r->size);
    // 更新符号，当新数据为零或被除数与除数符号相同时取正号
    q->sign = iszero(q) || ((x->sign ^ y->sign) == 0) ? 1 : -1;
    // 当被除数和除数符号不同且余数不为零时
    if(!((x->sign ^ y->sign) == 0) && !iszero(r))
    {
        // 定义借位项，调用扩展精度一位减法函数
        int borrow = XP_sub(r->size, r->digits, y->digits, r->digits, 0);

        // 判断借位项是否为零
        assert(borrow == 0);
        // 调用静态任意函数归一化函数更新使用位数
        normalize(r, r->size);
    }
    // 调用任意精度释放函数，释放商数据
    AP_free(&q);

    // 返回余数数据
    return r;
}

// 定义任意精度乘方函数
T AP_pow(T x, T y, T p)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 判断次方项是否为正数
    assert(y->sign == 1);
    // 判断除数项是否为零，当不为零时，需保证该项为正数且不能为零或为一
    assert(!p || p->sign == 1 && !iszero(p) && !isone(p));
    // 当底数为零时，结果为零
    if(iszero(x))
        return AP_new(0);
    // 当指数为零时，结果为一
    if(iszero(y))
        return AP_new(1);
    // 当底数的绝对值为一时，结果会根据指数的奇偶和底数的正负得出一或负一
    if(isone(x))
        return AP_new((((y)->digits[0] & 1) == 0) ? 1 : x->sign);
    // 若除数项存在
    if(p)
    {
        // 若指数项为一
        if(isone(y))
            // 结果为底数对除数项取余
            z = AP_mod(x, p);
        // 若指数项不为一
        else
        {
            // 这里采用快速幂取模算法
            // 快速幂算法采用二分指数方法，同时使用递归，将时间复杂度降至O(logn)
            // 还利用取模运算性质：a^b mod p = (a mod p)^b mod p，这里‘^’符号表示乘方
            // 定义二分之一指数项并使用该项进行乘方计算
            T y2 = AP_rshift(y, 1), t = AP_pow(x, y2, p);

            // 调用静态乘法取模函数计算结果
            z = mulmod(t, t, p);
            // 释放二分之一指数项
            AP_free(&y2);
            // 释放使用二分之一指数项的乘方结果
            AP_free(&t);
            // 若指数项绝对值为奇数
            if(!(((y)->digits[0] & 1) == 0))
            {
                // 调用静态乘法取模函数进行最后一次乘方计算
                z = mulmod(y2 = AP_mod(x, p), t = z, p);
                // 释放二分之一指数项
                AP_free(&y2);
                // 释放使用二分之一指数项的乘方结果
                AP_free(&t);
            }
        }
    }
    // 若除数项不存在
    else
    {
        // 若指数项为一
        if(isone(y))
            // 调用任意精度简易加法函数计算结果
            z = AP_addi(x, 0);
        // 若指数项不为一
        else
        {
            // 定义二分之一指数项并使用该项进行乘方计算
            T y2 = AP_rshift(y, 1), t = AP_pow(x, y2, NULL);

            // 调用任意精度乘法函数计算结果
            z = AP_mul(t, t);
            // 释放二分之一指数项
            AP_free(&y2);
            // 释放使用二分之一指数项的乘方结果
            AP_free(&t);
            if(!(((y)->digits[0] & 1) == 0))
            {
                // 调用任意精度乘法函数进行最后一次乘方计算
                z = AP_mul(x, t = z);
                // 释放使用二分之一指数项的乘方结果
                AP_free(&t);
            }
        }
    }

    // 返回新数据
    return z;
}

// 定义任意精度简易加法函数
T AP_addi(T x, long int y)
{
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;

    // 返回调用任意精度加法函数
    return AP_add(x, set(&t, y));
}

// 定义任意精度简易减法函数
T AP_subi(T x, long int y)
{
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;

    // 返回调用任意精度减法函数
    return AP_sub(x, set(&t, y));
}

// 定义任意精度简易乘法函数
T AP_muli(T x, long int y)
{
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;

    // 返回调用任意精度乘法函数
    return AP_mul(x, set(&t, y));
}

// 定义任意精度简易除法函数
T AP_divi(T x, long int y)
{
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;

    // 返回调用任意精度除法函数
    return AP_div(x, set(&t, y));
}

// 定义任意精度简易取模函数
long AP_modi(T x, long int y)
{
    // 声明余数项
    long int rem;
    // 声明新数据结构体指针
    T r;
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;
    // 调用任意精度取模函数
    r = AP_mod(x, set(&t, y));
    // 调用扩展精度整型表示函数，可以调用扩展精度的接口是因为余数必然为正数
    rem = XP_toint(r->ndigits, r->digits);
    // 释放新数据
    AP_free(&r);

    // 返回余数项
    return rem;
}

// 定义任意精度左移函数
T AP_lshift(T x, int s)
{
    // 声明新数据结构体指针
    T z;

    // 判断原数据是否合法
    assert(x);
    // 判断移动位数是否合法
    assert(s >= 0);
    // 对新数据分配内存空间，这里用原数使用位数加上移动位数转字节数向上取整
    z = mk(x->ndigits + ((s + 7) & ~7) / 8);
    // 调用扩展精度左移函数
    XP_lshift(z->size, z->digits, x->ndigits, x->digits, s, 0);
    // 更新符号位
    z->sign = x->sign;

    // 返回调用静态任意函数归一化函数更新使用位数
    return normalize(z, z->size);
}

// 定义任意精度右移函数
T AP_rshift(T x, int s)
{
    // 判断原数据是否合法
    assert(x);
    // 判断移动位数是否合法
    assert(s >= 0);
    // 若移动位数大于原数使用位数，结果直接为零
    if(s >= 8 * x->ndigits)
        return AP_new(0);
    else
    {
        // 定义新数据结构体指针并分配内存空间
        T z = mk(x->ndigits - s / 8);
        // 调用扩展精度右移函数
        XP_rshift(z->size, z->digits, x->ndigits, x->digits, s, 0);
        // 调用静态任意函数归一化函数更新使用位数
        normalize(z, z->size);
        // 更新符号位
        z->sign = iszero(z) ? 1 : x->sign;

        // 返回新数据
        return z;
    }
}

// 定义任意精度比较函数
int AP_cmp(T x, T y)
{
    // 判断原数据是否合法
    assert(x);
    assert(y);
    // 当两个数符号不同时，返回前一项的符号
    if(!((x->sign ^ y->sign) == 0))
        return x->sign;
    // 当两个数都是正数时
    else if(x->sign == 1)
        // 调用静态比较函数
        return cmp(x, y);
    // 当两个数都是负数时
    else
        // 调用静态比较函数，这里交换两数是为了保证结果是前一项减后一项
        return cmp(y, x);
}

// 定义任意精度简易比较函数
int AP_cmpi(T x, long int y)
{
    // 声明临时扩展精度数据
    unsigned char d[sizeof(unsigned long)];
    // 声明临时任意精度数据
    struct T t;

    // 更新总位数
    t.size = sizeof d;
    // 更新扩展精度数据指针
    t.digits = d;

    // 返回调用任意精度比较函数
    return AP_cmp(x, set(&t, y));
}

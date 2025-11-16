// 调库
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include "assert.h"
#include "except.h"
#include "fmt.h"
#include "mem.h"

#define T Fmt_T

// 定义字符串存储结构体
struct buf
{
    char *buf; // 该参数指向字符串首地址
    char *bp; // 该参数指向下一字符串首地址
    int size; // 该参数为字符串长度
};

// 定义格式化填充宏函数
#define pad(n, c) \
    do{ \
        int nn = (n); /* 转存字符数量 */ \
        while(nn-- > 0) \
            put((c), cl); /* 调用字符处理函数 */ \
    } while(0)

// 定义静态字符串型转换函数
static void cvt_s(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个字符串型量
    char *str = va_arg(*app, char *);

    // 判断该值是否合法
    assert(str);
    // 调用格式化字符串传递函数
    Fmt_puts(str, strlen(str), put, cl, flags, width, precision);
}

// 定义静态有符号整型十进制转换函数
static void cvt_d(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个整型量
    int val = va_arg(*app, int);
    // 设定一个无符号整型数字，用来修正参数
    unsigned m;
    // 设定一个魔数大小的字符串
    char buf[43];
    // 设定一个指向字符串后面一个地址的指针
    char *p = buf + sizeof buf;

    // 判断传入参数是否为最小整型数
    if(val == INT_MIN)
        // 将m修正为最大整型加一
        m = INT_MAX + 1U;
    // 判断传入参数是否为负数，是则取反
    else if(val < 0)
        m = -val;
    else
        m = val;
    do
        // 将修正值取每一位数字转为字符，存入字符指针指向的地址
        *--p = m % 10 + '0';
    // 循环判断修正值是否存完同时更新修正值
    while((m /= 10) > 0);
    // 当传入参数小于零时在字符串前补负号
    if(val < 0)
        *--p = '-';
    // 调用格式化数字传递函数
    Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags, width, precision);
}

// 定义静态无符号整型十进制转换函数
static void cvt_u(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个无符号整型量
    unsigned m = va_arg(*app, unsigned);
    // 设定一个魔数大小的字符串
    char buf[43];
    // 设定一个指向字符串后面一个地址的指针
    char *p = buf + sizeof buf;

    do
        // 取每一位数字转为字符，存入字符指针指向的地址
        *--p = m % 10 + '0';
    // 循环判断数字量是否存完并更新该值
    while((m /= 10) > 0);
    // 调用格式化数字传递函数
    Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags, width, precision);
}

// 定义静态无符号整型八进制转换函数
static void cvt_o(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个无符号整型量
    unsigned m = va_arg(*app, unsigned);
    // 设定一个魔数大小的字符串
    char buf[43];
    // 设定一个指向字符串后面一个地址的指针
    char *p = buf + sizeof buf;

    do
        // 取每一八进制位数字转为字符，存入字符指针指向的地址
        *--p = (m & 0x7) + '0';
    // 循环判断数字量是否存完并更新该值
    while((m >>= 3) != 0);
    // 调用格式化数字传递函数
    Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags, width, precision);
}

// 定义静态无符号整型十六进制转换函数
static void cvt_x(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个无符号整型量
    unsigned m = va_arg(*app, unsigned);
    // 设定一个魔数大小的字符串
    char buf[43];
    // 设定一个指向字符串后面一个地址的指针
    char *p = buf + sizeof buf;

    do
        // 取每一十六进制位数字转为字符，存入字符指针指向的地址
        *--p = "123456789abcdef"[m & 0xf];
    // 循环判断数字量是否存完并更新该值
    while((m >>= 4) != 0);
    // 调用格式化数字传递函数
    Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags, width, precision);
}

// 定义静态指针型转换函数
static void cvt_p(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 获取下一个无符号整型量并转换为无符号长整型
    unsigned long m = (unsigned long)va_arg(*app, unsigned);
    // 设定一个魔数大小的字符串
    char buf[43];
    // 设定一个指向字符串后面一个地址的指针
    char *p = buf + sizeof buf;

    // 更新精度为最小整型值
    precision = INT_MIN;
    do
        // 取每一十六进制位数字转为字符，存入字符指针指向的地址
        *--p = "123456789abcdef"[m & 0xf];
    // 循环判断数字量是否存完并更新该值
    while((m >>= 4) != 0);
    // 调用格式化数字传递函数
    Fmt_putd(p, (buf + sizeof buf) - p, put, cl, flags, width, precision);
}

// 定义静态字符型转换函数
static void cvt_c(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 当宽度为最小整型值时，更新为0
    if(width == INT_MIN)
        width = 0;
    // 当宽度为负数时
    if(width < 0)
    {
        // 负号标志位更新
        flags['-'] = 1;
        // 宽度取反
        width = -width;
    }
    // 填充空格使字符右对齐
    if(!flags['-'])
        pad(width - 1, ' ');
    // 处理字符
    put((unsigned char)va_arg(*app, int), cl);
    // 填充空格使字符左对齐
    if(flags['-'])
        pad(width - 1, ' ');
}

// 定义静态浮点型转换函数
static void cvt_f(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 定义一个能存储双精度浮点型的字符串
    // 这里包括小数点前的DBL_MAX_10_EXP+1，小数点的1位，小数点后99位，终止符1位
    char buf[DBL_MAX_10_EXP + 1 + 1 + 99 + 1];

    // 如果精度为负数，更新为6
    if(precision < 0)
        precision = 6;
    // 如果占位符为g且精度为零，更新精度为1
    if(code == 'g' && precision == 0)
        precision = 1;

    {
        // 定义静态格式化字符串
        static char fmt[] = "%.dd?";
        // 判断精度是否合法
        assert(precision <= 99);
        // 更新上面静态格式化字符串的内容，将其适配于标准库格式
        fmt[4] = code;
        fmt[3] = precision % 10 + '0';
        fmt[2] = (precision / 10) % 10 + '0';
        // 调用标准库字符串输出函数
        sprintf(buf, fmt, va_arg(*app, double));
    }
}

// 定义格式化溢出异常
const Except_T Fmt_Overflow = {"Formatting Overflow"};

// 定义静态转换函数指针数组
static T cvt[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, cvt_c, cvt_d, cvt_f, cvt_f, cvt_f,
    0, 0, 0, 0, 0, 0, 0, cvt_o,
    cvt_p, 0, 0, cvt_s, 0, cvt_u, 0, 0,
    cvt_x, 0, 0, 0, 0, 0, 0, 0,
};

// 定义格式化标志
char *Fmt_flags = "-+ 0";

// 定义静态字符输出函数
static int outc(int c, void *cl)
{
    // 将空指针型转换为流指针型
    FILE *f = cl;

    // 返回调用标准库字符输出函数
    return putc(c, f);
}

// 定义静态字符插入函数
static int insert(int c, void *cl)
{
    // 将空指针型转换为字符串存储结构体指针型
    struct buf *p = cl;

    // 若下一字符串地址大于等于字符串末尾地址，则抛出溢出异常
    if(p->bp >= p->buf + p->size)
        RAISE(Fmt_Overflow);
    // 插入字符，并更新下一字符串地址
    *p->bp++ = c;

    // 返回字符
    return c;
}

// 定义静态字符附加函数
static int append(int c, void *cl)
{
    // 将空指针型转换为字符串存储结构体指针型
    struct buf *p = cl;

    // 若下一字符串地址大于等于字符串末尾地址，则扩大字符串池
    if(p->bp >= p->buf + p->size)
    {
        // 重分配地址，扩大为二倍
        RESIZE(p->buf, 2 * p->size);
        // 更新下一字符串地址
        p->bp = p->buf + p->size;
        // 更新字符串大小
        p->size *= 2;
    }
    // 插入字符，并更新下一字符串地址
    *p->bp++ = c;

    // 返回字符
    return c;
}

// 定义格式化函数
void Fmt_fmt(int put(int c, void *cl), void *cl, const char *fmt, ...)
{
    // 声明可变参数列表
    va_list ap;

    // 初始化可变参数列表
    va_start(ap, fmt);
    // 调用带可变参数数组的格式化函数
    // 自定输出函数，自定输出流
    Fmt_vfmt(put, cl, fmt, ap);
    // 结束可变参数列表的访问
    va_end(ap);
}

// 定义格式化函数（带可变参数数组）
void Fmt_vfmt(int put(int c, void *cl), void *cl, const char *fmt, va_list ap)
{
    // 判断字符处理函数是否合法
    assert(put);
    // 判断需格式化的字符串是否合法
    assert(fmt);
    // 循环判断字符
    while(*fmt)
    {
        // 判断当前字符是否为'%'，如果是，判断下一位字符也是
        // 在前面语句为是时，直接执行，当为否时才判断后面语句，符号特性，精妙！
        if(*fmt != '%' || *++fmt == '%')
            // 调用字符处理函数并更新字符
            put((unsigned char)*fmt++, cl);
        // 上面条件不满足时，则需要进行字符串替换
        else
        {
            // 定义无符号字符型变量和数组
            unsigned char c, flags[256];
            // 定义宽度和精度变量，并更新为整型最小值
            int width = INT_MIN, precision = INT_MIN;
            // 将无符号字符型数组内全更新为空字符
            memset(flags, '\0', sizeof flags);
            // 获取可选标志
            if(Fmt_flags)
            {
                // 获取当前字符
                unsigned char c = *fmt;
                // 循环判断字符是否存在且是否在格式化标志中
                for(; c && strchr(Fmt_flags, c); c = *++fmt)
                {
                    // 判断无符号字符型数组在当前字符位置是否超限
                    assert(flags[c] < 255);
                    // 自增无符号字符型数组在当前字符位置
                    flags[c]++;
                }
            }
            // 获取宽度
            if(*fmt == '*' || isdigit(*fmt))
            {
                // 声明临时宽度变量
                int n;
                // 若当前字符为星号
                if(*fmt == '*')
                {
                    // 记录可变参数列表中的下一个整型量
                    n = va_arg(ap, int);
                    // 判断该量是否为整型最小值
                    assert(n != INT_MIN);
                    // 移动至下一字符
                    fmt++;
                }
                else
                {
                    // 循环判断字符是否是数字
                    for(n = 0; isdigit(*fmt); fmt++)
                    {
                        // 将字符转换为数字
                        int d = *fmt - '0';
                        // 判断记录上当前字符时候是否超过整型限幅
                        assert(n <= (INT_MAX - d) / 10);
                        // 更新临时宽度
                        n = 10 * n + d;
                    }
                }
                // 更新宽度
                width = n;
            }
            // 获取精度
            if(*fmt == '.' && (*++fmt == '*' || isdigit(*fmt)))
            {
                // 声明临时精度变量
                int n;

                if(*fmt == '*')
                {
                    // 记录可变参数列表中的下一个整型量
                    n = va_arg(ap, int);
                    // 判断该量是否为整型最小值
                    assert(n != INT_MIN);
                    // 移动至下一字符
                    fmt++;
                }
                else
                {
                    // 循环判断字符是否是数字
                    for(n = 0; isdigit(*fmt); fmt++)
                    {
                        // 将字符转换为数字
                        int d = *fmt - '0';
                        // 判断记录上当前字符时候是否超过整型限幅
                        assert(n <= (INT_MAX - d) / 10);
                        // 更新临时宽度
                        n = 10 * n + d;
                    }
                }
                // 更新精度
                precision = n;
            }
            // 获取占位字符
            c = *fmt++;
            // 判断该字符下转换函数是否存在
            assert(cvt[c]);
            // 调用转换函数
            (*cvt[c])(c, &ap, put, cl, flags, width, precision);
        }
    }
}

// 定义字符串格式化函数（标准输出流）
void Fmt_print(const char *fmt, ...)
{
    // 声明可变参数列表
    va_list ap;

    // 初始化可变参数列表
    va_start(ap, fmt);
    // 调用带可变参数数组的格式化函数
    // 指定输出函数，标准输出流
    Fmt_vfmt(outc, stdout, fmt, ap);
    // 结束可变参数列表的访问
    va_end(ap);
}

// 定义字符串格式化函数（文件流）
void Fmt_fprint(FILE *stream, const char *fmt, ...)
{
    // 声明可变参数列表
    va_list ap;

    // 初始化可变参数列表
    va_start(ap, fmt);
    // 调用带可变参数数组的格式化函数
    // 指定输出函数，自定输出流
    Fmt_vfmt(outc, stream, fmt, ap);
    // 结束可变参数列表的访问
    va_end(ap);
}

// 定义字符串格式化函数（字符串存储）
int Fmt_sfmt(char *buf, int size, const char *fmt, ...)
{
    // 声明可变参数列表
    va_list ap;
    // 声明长度变量
    int len;

    // 初始化可变参数列表
    va_start(ap, fmt);
    // 调用带可变参数数组的格式化函数
    len = Fmt_vsfmt(buf, size, fmt, ap);
    // 结束可变参数列表的访问
    va_end(ap);

    // 返回字符串长度
    return len;
}

// 定义字符串格式化函数（字符串存储）（带可变参数数组）
int Fmt_vsfmt(char *buf, int size, const char *fmt, va_list ap)
{
    // 声明字符串存储结构体
    struct buf cl;

    // 判断字符串是否合法
    assert(buf);
    // 判断字符串长度是否合法
    assert(size > 0);
    // 判断需格式化的字符串是否合法
    assert(fmt);
    // 更新字符串存储结构体中的内容
    cl.buf = cl.bp = buf;
    cl.size = size;
    // 调用带可变参数数组的格式化函数
    // 指定插入函数，指定被插入字符串地址
    Fmt_vfmt(insert, &cl, fmt, ap);
    // 字符串末尾插入空字符
    insert(0, &cl);

    // 返回字符串长度（不算空字符）
    return cl.bp - cl.buf - 1;
}

// 定义字符串格式化函数（字符串池）
char *Fmt_string(const char *fmt, ...)
{
    // 声明字符串
    char *str;
    // 声明可变参数列表
    va_list ap;

    // 判断需格式化的字符串是否合法
    assert(fmt);
    // 初始化可变参数列表
    va_start(ap, fmt);
    // 调用带可变参数数组的格式化函数
    str = Fmt_vstring(fmt, ap);
    // 结束可变参数列表的访问
    va_end(ap);

    // 返回字符串
    return str;
}

// 定义字符串格式化函数（字符串池）（带可变参数数组）
char *Fmt_vstring(const char *fmt, va_list ap)
{
    // 声明字符串存储结构体
    struct buf cl;

    // 判断需格式化的字符串是否合法
    assert(fmt);
    // 更新字符串存储结构体中的内容
    cl.size = 256;
    // 分配内存空间
    cl.buf = cl.bp = ALLOC(cl.size);
    // 调用带可变参数数组的格式化函数
    // 指定附加函数，指定被插入字符串地址
    Fmt_vfmt(append, &cl, fmt, ap);
    // 字符串末尾附加空字符
    append(0, &cl);

    // 返回重新分配地址
    return RESIZE(cl.buf, cl.bp - cl.buf);
}

// 定义格式化转换函数注册函数
T Fmt_register(int code, T newcvt)
{
    // 声明旧转换函数指针
    T old;

    // 判断占位字符是否合法
    assert(0 < code && code < (int)(sizeof(cvt) / sizeof(cvt[0])));
    // 记录旧转换函数
    old = cvt[code];
    // 更新转换函数
    cvt[code] = newcvt;

    // 返回旧转换函数
    return old;
}

// 定义格式化数字传递函数
void Fmt_putd(const char *str, int len, int put(int c, void *cl), void *cl, unsigned char flags[256], int width, int precision)
{
    // 定义符号变量
    int sign;

    // 判断需格式化的字符串是否合法
    assert(str);
    // 判断字符串长度是否合法
    assert(len >= 0);
    // 判断格式化标志是否合法
    assert(flags);
    // 当宽度为最小整型值时，更新为0
    if(width == INT_MIN)
        width = 0;
    // 当宽度为负数时
    if(width < 0)
    {
        // 负号标志位更新
        flags['-'] = 1;
        // 宽度取反
        width = -width;
    }
    // 当精度为正数时
    if(precision >= 0)
        // 零标志位更新
        flags['0'] = 0;

    // 如果字符串长度为正数，且首位为负号或正号
    if(len > 0 && (*str == '-' || *str == '+'))
    {
        // 更新符号变量，同时更新原字符串
        sign = *str++;
        // 字符串长度自减
        len--;
    }
    // 如果标志位中正号位被标记
    else if(flags['+'])
        // 符号变量更新为正号
        sign = '+';
    // 如果标志位中空格位被标记
    else if(flags[' '])
        // 符号变量更新为空格
        sign = ' ';
    else
        // 其他情况下符号位为零
        sign = 0;
    {
        // 声明计数变量
        int n;
        // 如果精度为负数，更新为1
        if(precision < 0)
            precision = 1;
        // 如果精度大于字符串长度
        if(len < precision)
            // 更新计数变量为精度
            n = precision;
        // 如果精度为零，且字符串长度为一，字符串首字符为'0'
        else if(precision == 0 && len == 1 && str[0] == '0')
            // 更新计数变量为零
            n = 0;
        else
            // 其他情况下计数变量为字符串长度
            n = len;
        // 如果存在符号变量
        if(sign)
            // 计数变量自增
            n++;
        // 如果标志位中负号位被标记
        if(flags['-'])
        {
            // 如果符号变量存在
            if(sign)
                // 输出符号
                put(sign, cl);
        }
        // 如果标志位中零标志位被标记
        else if(flags['0'])
        {
            // 如果符号变量存在
            if(sign)
                // 输出符号
                put(sign, cl);
            // 填充零字符
            pad(width - n, '0');
        }
        else
        {
            // 填充空格字符
            pad(width - n, ' ');
            // 如果符号变量存在
            if(sign)
                // 输出符号
                put(sign, cl);
        }
        // 填充零字符
        pad(precision - len, '0');
        {
            // 声明计数变量
            int i;
            // 遍历字符串，调用字符处理函数处理各字符
            for(i = 0; i < len; i++)
                put((unsigned char)*str++, cl);
        }
        // 如果标志位中负号位被标记
        if(flags['-'])
            // 填充空格字符
            pad(width - n, ' ');
    }
}

// 定义格式化字符串传递函数
void Fmt_puts(const char *str, int len, int put(int c, void *cl), void *cl, unsigned char flags[256], int width, int precision)
{
    // 判断需格式化的字符串是否合法
    assert(str);
    // 判断字符串长度是否合法
    assert(len >= 0);
    // 判断格式化标志是否合法
    assert(flags);
    // 当宽度为最小整型值时，更新为0
    if(width == INT_MIN)
        width = 0;
    // 当宽度为负数时
    if(width < 0)
    {
        // 负号标志位更新
        flags['-'] = 1;
        // 宽度取反
        width = -width;
    }
    // 当精度为正数时
    if(precision >= 0)
        // 零标志位更新
        flags['0'] = 0;

    // 如果精度大于等于零且小于字符串长度
    if(precision >= 0 && precision < len)
        // 更新字符串长度为精度值
        len = precision;
    // 如果标志位中负号位未被标记
    if(!flags['-'])
        // 填充空格字符
        pad(width - len, ' ');
    {
        // 声明计数变量
        int i;
        // 遍历字符串，调用字符处理函数处理各字符
        for(i = 0; i < len; i++)
            put((unsigned char)*str++, cl);
    }
    // 如果标志位中负号位被标记
    if(flags['-'])
        // 填充空格字符
        pad(width - len, ' ');
}

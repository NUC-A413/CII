#ifndef AP_INCLUDED
#define AP_INCLUDED

#include <stdarg.h>

// 库内简化写法
#define T AP_T

// 声明任意精度数据结构体隐性指针
typedef struct T *T;

// 声明任意精度新建函数
extern T AP_new(long int n);
// 声明任意精度字符串获取函数
extern T AP_fromstr(const char *str, int base, char **end);
// 声明任意精度整型表示函数
extern long int AP_toint(T x);
// 声明任意精度字符串表示函数
extern char *AP_tostr(char *str, int size, int base, T x);
// 声明任意精度格式化函数
extern void AP_fmt(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision);
// 声明任意精度释放函数
extern void AP_free(T *z);
// 声明任意精度取负函数
extern T AP_neg(T x);
// 声明任意精度加法函数
extern T AP_add(T x, T y);
// 声明任意精度减法函数
extern T AP_sub(T x, T y);
// 声明任意精度乘法函数
extern T AP_mul(T x, T y);
// 声明任意精度除法函数
extern T AP_div(T x, T y);
// 声明任意精度取模函数
extern T AP_mod(T x, T y);
// 声明任意精度乘方函数
extern T AP_pow(T x, T y, T p);
// 声明任意精度简易加法函数
extern T AP_addi(T x, long int y);
// 声明任意精度简易减法函数
extern T AP_subi(T x, long int y);
// 声明任意精度简易乘法函数
extern T AP_muli(T x, long int y);
// 声明任意精度简易除法函数
extern T AP_divi(T x, long int y);
// 声明任意精度简易取模函数
extern long AP_modi(T x, long int y);
// 声明任意精度左移函数
extern T AP_lshift(T x, int s);
// 声明任意精度右移函数
extern T AP_rshift(T x, int s);
// 声明任意精度比较函数
extern int AP_cmp(T x, T y);
// 声明任意精度简易比较函数
extern int AP_cmpi(T x, long int y);


#undef T
#endif

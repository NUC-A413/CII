#ifndef XP_INCLUDED
#define XP_INCLUDED

// 库内简化写法
#define T XP_T

// 声明扩展精度数据格式
typedef unsigned char *T;

// 声明进位加法函数
extern int XP_add(int n, T z, T x, T y, int carry);
// 声明借位减法函数
extern int XP_sub(int n, T z, T x, T y, int borrow);
// 声明乘法函数
extern int XP_mul(T z, int n, T x, int m, T y);
// 声明除法函数
extern int XP_div(int n, T q, T x, int m, T y, T r, T tmp);
// 声明一位加法函数
extern int XP_sum(int n, T z, T x, int y);
// 声明一位减法函数
extern int XP_diff(int n, T z, T x, int y);
// 声明一位乘法函数
extern int XP_product(int n, T z, T x, int y);
// 声明一位除法函数
extern int XP_quotient(int n, T z, T x, int y);
// 声明取反取补函数
extern int XP_neg(int n, T z, T x, int carry);
// 声明比较函数
extern int XP_cmp(int n, T x, T y);
// 声明左移函数
extern void XP_lshift(int n, T z, int m, T x, int s, int fill);
// 声明右移函数
extern void XP_rshift(int n, T z, int m, T x, int s, int fill);
// 声明数字位数函数
extern int XP_length(int n, T x);
// 声明整型获取函数
extern unsigned long XP_fromint(int n, T z, unsigned long u);
// 声明整型表示函数
extern unsigned long XP_toint(int n, T x);
// 声明字符串获取函数
extern int XP_fromstr(int n, T z, const char *str, int base, char **end);
// 声明字符串表示函数
extern char *XP_tostr(char *str, int size, int base, int n, T x);

#undef T
#endif

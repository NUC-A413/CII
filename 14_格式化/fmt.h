#ifndef FMT_INCLUDED
#define FMT_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include "except.h"

#define T Fmt_T

// 声明格式化函数指针
typedef void (*T)(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[256], int width, int precision);

// 声明格式化标志
extern char *Fmt_flags;
// 声明格式化溢出异常
extern const Except_T Fmt_Overflow;

// 声明格式化函数
extern void Fmt_fmt(int put(int c, void *cl), void *cl, const char *fmt, ...);
// 声明格式化函数（带可变参数数组）
extern void Fmt_vfmt(int put(int c, void *cl), void *cl, const char *fmt, va_list ap);
// 声明字符串格式化函数（标准输出流）
extern void Fmt_print(const char *fmt, ...);
// 声明字符串格式化函数（文件流）
extern void Fmt_fprint(FILE *stream, const char *fmt, ...);
// 声明字符串格式化函数（字符串存储）
extern int Fmt_sfmt(char *buf, int size, const char *fmt, ...);
// 声明字符串格式化函数（字符串存储）（带可变参数数组）
extern int Fmt_vsfmt(char *buf, int size, const char *fmt, va_list ap);
// 声明字符串格式化函数（字符串池）
extern char *Fmt_string(const char *fmt, ...);
// 声明字符串格式化函数（字符串池）（带可变参数数组）
extern char *Fmt_vstring(const char *fmt, va_list ap);
// 声明格式化转换函数注册函数
extern T Fmt_register(int code, T newcvt);
// 声明格式化数字传递函数
extern void Fmt_putd(const char *str, int len, int put(int c, void *cl), void *cl, unsigned char flags[256], int width, int precision);
// 声明格式化字符串传递函数
extern void Fmt_puts(const char *str, int len, int put(int c, void *cl), void *cl, unsigned char flags[256], int width, int precision);

#undef T
#endif

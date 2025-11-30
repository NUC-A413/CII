#ifndef STR_INCLUDED
#define STR_INCLUDED

#include <stdarg.h>

// 声明低级字符串子字符串函数
extern char *Str_sub(const char *s, int i, int j);
// 声明低级字符串子字符串复制函数
extern char *Str_dup(const char *s, int i, int j, int n);
// 声明低级字符串子字符串拼接函数
extern char *Str_cat(const char *s1, int i1, int j1, const char *s2, int i2, int j2);
// 声明低级字符串子字符串拼接函数（带可变参数数组）
extern char *Str_catv(const char *s, ...);
// 声明低级字符串子字符串反转函数
extern char *Str_reverse(const char *s, int i, int j);
// 声明低级字符串子字符串映射函数
extern char *Str_map(const char *s, int i, int j, const char *from, const char *to);
// 声明低级字符串子字符串正位置函数
extern int Str_pos(const char *s, int i);
// 声明低级字符串子字符串字符数函数
extern int Str_len(const char *s, int i, int j);
// 声明低级字符串子字符串比较函数
extern int Str_cmp(const char *s1, int i1, int j1, const char *s2, int i2, int j2);
// 声明低级字符串字符左查找函数
extern int Str_chr(const char *s, int i, int j, int c);
// 声明低级字符串字符右查找函数
extern int Str_rchr(const char *s, int i, int j, int c);
// 声明低级字符串字符集合左查找函数
extern int Str_upto(const char *s, int i, int j, const char *set);
// 声明低级字符串字符集合右查找函数
extern int Str_rupto(const char *s, int i, int j, const char *set);
// 声明低级字符串字符串左查找函数
extern int Str_find(const char *s, int i, int j, const char *str);
// 声明低级字符串字符串右查找函数
extern int Str_rfind(const char *s, int i, int j, const char *str);
// 声明低级字符串字符集合存在函数
extern int Str_any(const char *s, int i, const char *set);
// 声明低级字符串字符串头部集合存在函数
extern int Str_many(const char *s, int i, int j, const char *set);
// 声明低级字符串字符串尾部集合存在函数
extern int Str_rmany(const char *s, int i, int j, const char *set);
// 声明低级字符串字符串头部匹配函数
extern int Str_match(const char *s, int i, int j, const char *str);
// 声明低级字符串字符串尾部集合存在函数
extern int Str_rmatch(const char *s, int i, int j, const char *str);

#endif

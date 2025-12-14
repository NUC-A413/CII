#ifndef TEXT_H
#define TEXT_H

#include <stdarg.h>

// 库内简化写法
#define T Text_T

// 定义高级字符串结构体
typedef struct T
{
    int len;
    const char *str;
} T;

// 声明高级字符串存储结构体指针
typedef struct Text_save_T *Text_save_T;

// 声明常量字符串集合
// 声明全字符集合
extern const T Text_cset;
// 声明ASCII字符集合
extern const T Text_ascii;
// 声明大写字母字符集合
extern const T Text_ucase;
// 声明小写字母字符集合
extern const T Text_lcase;
// 声明数字字符集合
extern const T Text_digits;
// 声明无字符集合
extern const T Text_null;

// 声明高级字符串创建函数
extern T Text_put(const char *str);
// 声明高级字符串获取函数
extern char *Text_get(char *str, int size, T s);
// 声明高级字符串打包函数
extern T Text_box(const char *str, int len);
// 声明高级字符串子字符串函数
extern T Text_sub(T s, int i, int j);
// 声明高级字符串子字符串正位置函数
extern int Text_pos(T s, int i);
// 声明高级字符串子字符串拼接函数
extern T Text_cat(T s1, T s2);
// 声明高级字符串子字符串复制函数
extern T Text_dup(T s, int n);
// 声明高级字符串子字符串反转函数
extern T Text_reverse(T s);
// 声明高级字符串子字符串映射函数
extern T Text_map(T s, const T *from, const T *to);
// 声明高级字符串子字符串比较函数
extern int Text_cmp(T s1, T s2);
// 声明高级字符串字符左查找函数
extern int Text_chr(T s, int i, int j, int c);
// 声明高级字符串字符右查找函数
extern int Text_rchr(T s, int i, int j, int c);
// 声明高级字符串字符集合左查找函数
extern int Text_upto(T s, int i, int j, T set);
// 声明高级字符串字符集合右查找函数
extern int Text_rupto(T s, int i, int j, T set);
// 声明高级字符串字符集合存在函数
extern int Text_any(T s, int i, T set);
// 声明高级字符串字符串头部集合存在函数
extern int Text_many(T s, int i, int j, T set);
// 声明高级字符串字符串尾部集合存在函数
extern int Text_rmany(T s, int i, int j, T set);
// 声明高级字符串字符串左查找函数
extern int Text_find(T s, int i, int j, T str);
// 声明高级字符串字符串右查找函数
extern int Text_rfind(T s, int i, int j, T str);
// 声明高级字符串字符串头部匹配函数
extern int Text_match(T s, int i, int j, T str);
// 声明高级字符串字符串尾部匹配函数
extern int Text_rmatch(T s, int i, int j, T str);
// 声明高级字符串格式化函数
extern void Text_fmt(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision);
// 声明高级字符串存储函数
extern Text_save_T Text_save(void);
// 声明高级字符串恢复函数
extern void Text_restore(Text_save_T *save);

#undef T
#endif

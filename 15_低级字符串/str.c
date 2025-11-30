// 调库
#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "str.h"
#include "mem.h"

// 定义字符位置至索引转换宏函数
#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i) - 1)
// 定义字符串位置至索引转换宏函数
#define convert(s, i, j) \
    do{ \
        int len; /* 声明字符串长度变量 */ \
        assert(s); /* 判断字符串是否合法 */ \
        len = strlen(s); /* 获取字符串长度 */ \
        i = idx(i, len); \
        j = idx(j, len); /* 将字符串位置转换为索引 */ \
        if(i > j){ /* 若前后索引大小相反，则交换 */ \
            int t = i; \
            i = j; \
            j = t; \
        } \
        assert(i >= 0 && j <= len); /* 判断索引值是否合法 */ \
    } while(0)

// 定义低级字符串子字符串函数
char *Str_sub(const char *s, int i, int j)
{
    // 声明新字符串和字符指针
    char *str, *p;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 分配内存空间
    p = str = ALLOC(j - i + 1);
    // 从原字符串中取出子字符串
    while(i < j)
        *p++ = s[i++];
    // 最后一位赋值空字符
    *p = '\0';

    // 返回新字符串
    return str;
}

// 定义低级字符串子字符串复制函数
char *Str_dup(const char *s, int i, int j, int n)
{
    // 声明计数变量
    int k;
    // 声明新字符串和字符指针
    char *str, *p;

    // 判断复制次数是否合法
    assert(n >= 0);
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 分配内存空间
    p = str = ALLOC(n * (j - i) + 1);
    // 从原字符串中重复取出子字符串
    if(j - i > 0)
        while(n-- > 0)
            for(k = i; k < j;k++)
                *p++ = s[k];
    // 最后一位赋值空字符
    *p = '\0';

    // 返回新字符串
    return str;
}

// 定义低级字符串子字符串拼接函数
char *Str_cat(const char *s1, int i1, int j1, const char *s2, int i2, int j2)
{
    // 声明新字符串和字符指针
    char *str, *p;

    // 将字符串位置转换为可用索引值
    convert(s1, i1, j1);
    convert(s2, i2, j2);
    // 分配内存空间
    p = str = ALLOC(j1 - i1 + j2 - i2 + 1);
    // 从两个原字符串中依次取出字符
    while(i1 < j1)
        *p++ = s1[i1++];
    while(i2 < j2)
        *p++ = s2[i2++];
    // 最后一位赋值空字符
    *p = '\0';

    // 返回新字符串
    return str;
}

// 定义低级字符串子字符串拼接函数（带可变参数数组）
char *Str_catv(const char *s, ...)
{
    // 声明新字符串和字符指针
    char *str, *p;
    // 定义字符串备份变量
    const char *save = s;
    // 定义计数变量
    int i, j, len = 0;
    // 声明可变参数列表
    va_list ap;

    // 初始化可变参数列表
    va_start(ap, s);
    // 获取最终新字符串的长度
    while(s)
    {
        // 从可变参数列表中获取字符串前后位置
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        // 将字符串位置转换为可用索引值
        convert(s, i, j);
        // 增加新字符串长度
        len += j - i;
        // 更新至下一要拼接的字符串
        s = va_arg(ap, const char *);
    }
    // 结束可变参数列表的访问
    va_end(ap);
    // 分配内存空间
    p = str = ALLOC(len + 1);
    // 取出最开始备份的字符串，为下一次遍历可变参数列表做准备
    s = save;
    // 初始化可变参数列表
    va_start(ap, s);
    // 依次拷贝字符串到新字符串中
    while(s)
    {
        // 从可变参数列表中获取字符串前后位置
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        // 将字符串位置转换为可用索引值
        convert(s, i, j);
        // 依次拷贝字符串到新字符串中
        while(i < j)
            *p++ = s[i++];
        // 更新至下一要拼接的字符串
        s = va_arg(ap, const char *);
    }
    // 结束可变参数列表的访问
    va_end(ap);
    // 最后一位赋值空字符
    *p = '\0';

    // 返回新字符串
    return str;
}

// 定义低级字符串子字符串反转函数
char *Str_reverse(const char *s, int i, int j)
{
    // 声明新字符串和字符指针
    char *str, *p;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 分配内存空间
    p = str = ALLOC(j - i + 1);
    // 从原字符串中取出子字符串
    while(j > i)
        *p++ = s[--j];
    // 最后一位赋值空字符
    *p = '\0';

    // 返回新字符串
    return str;
}

// 定义低级字符串子字符串映射函数
char *Str_map(const char *s, int i, int j, const char *from, const char *to)
{
    // 定义静态字符映射表，这里映射表长度定为256，匹配所有的256个的字符
    static char map[256] = {0};

    // 若映射字符串存在，则需要更新字符映射表
    if(from && to)
    {
        // 声明计数字符
        unsigned c;
        // 沿字符映射表遍历，将对应位刷新为对应字符串
        for(c = 0; c < sizeof map; c++)
            map[c] = c;
        // 遍历映射字符串，更新字符映射表
        while(*from && *to)
            map[(unsigned char)*from++] = *to++;
        // 判断映射字符串是否遍历结束
        assert(*from == 0 && *to == 0);
    }
    else
    {
        // 判断映射字符串是否为空且原字符串是否存在
        assert(from == NULL && to == NULL && s);
        // 利用非零字符判断第一次调用该函数时候映射字符串是否为空
        assert(map['a']);
    }
    if(s)
    {
        // 声明新字符串和字符指针
        char *str, *p;

        // 将字符串位置转换为可用索引值
        convert(s, i, j);
        // 分配内存空间
        p = str = ALLOC(j - i + 1);
        // 从原字符串中取出子字符串并做映射
        while(i < j)
            *p++ = map[(unsigned char)s[i++]];
        // 最后一位赋值空字符
        *p = '\0';

        // 返回新字符串
        return str;
    }
    else
        // 若原字符串不存在，则返回空指针
        return NULL;
}

// 定义低级字符串子字符串正位置函数
int Str_pos(const char *s, int i)
{
    // 声明字符串长度变量
    int len;

    // 判断字符串是否合法
    assert(s);
    // 获取字符串长度
    len = strlen(s);
    // 将字符串位置转换为索引
    i = idx(i, len);
    // 判断索引值是否合法
    assert(i >= 0 && i <= len);

    // 再将索引值转为位置值，并返回
    return i + 1;
}

// 定义低级字符串子字符串字符数函数
int Str_len(const char *s, int i, int j)
{
    // 将字符串位置转换为可用索引值
    convert(s, i, j);

    // 返回字符串索引值差值作为字符数量
    return j - i;
}

// 定义低级字符串子字符串比较函数
int Str_cmp(const char *s1, int i1, int j1, const char *s2, int i2, int j2)
{
    // 将字符串位置转换为可用索引值
    convert(s1, i1, j1);
    convert(s2, i2, j2);
    // 更新字符指针位置
    s1 += i1;
    s2 += i2;
    // 比较子字符串长度
    if(j1 - i1 < j2 - i2)
    {
        // 调用标准库子字符串比较函数
        int cond = strncmp(s1, s2, j1 - i1);

        // 返回比较结果，这里对字符串长度不同也做了处理
        return cond == 0 ? -1 : cond;
    }
    else if(j1 - i1 > j2 - i2)
    {
        // 调用标准库子字符串比较函数
        int cond = strncmp(s1, s2, j2 - i2);

        // 返回比较结果，这里对字符串长度不同也做了处理
        return cond == 0 ? +1 : cond;
    }
    else
        // 调用标准库子字符串比较函数并返回比较结果
        return strncmp(s1, s2, j1 - i1);
}

// 定义低级字符串字符左查找函数
int Str_chr(const char *s, int i, int j, int c)
{
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 遍历子字符串进行字符查找
    for(; i < j;i++)
        if(s[i] == c)
            // 再将索引值转为位置值，并返回
            return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符右查找函数
int Str_rchr(const char *s, int i, int j, int c)
{
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 遍历子字符串进行字符查找，从末尾向前
    while(j > i)
        if(s[--j] == c)
            // 再将索引值转为位置值，并返回
            return j + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符集合左查找函数
int Str_upto(const char *s, int i, int j, const char *set)
{
    // 判断集合字符串是否合法
    assert(set);
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 遍历子字符串进行字符查找
    for(;i < j;i++)
        if(strchr(set, s[i]))
            // 再将索引值转为位置值，并返回
            return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符集合右查找函数
int Str_rupto(const char *s, int i, int j, const char *set)
{
    // 判断集合字符串是否合法
    assert(set);
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 遍历子字符串进行字符查找，从末尾向前
    while(j > i)
        if(strchr(set, s[--j]))
            // 再将索引值转为位置值，并返回
            return j + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串左查找函数
int Str_find(const char *s, int i, int j, const char *str)
{
    // 声明字符串长度变量
    int len;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 判断查找字符串是否合法
    assert(str);
    // 获取查找字符串长度
    len = strlen(str);
    // 若查找字符串无内容
    if(len == 0)
        // 返回子字符串起始位置
        return i + 1;
    // 若查找字符串仅含有一个字符
    else if(len == 1)
    {
        // 遍历子字符串进行字符查找
        for(;i < j;i++)
            if(s[i] == *str)
                // 再将索引值转为位置值，并返回
                return i + 1;
    }
    // 若查找字符串包含多个字符
    else
        // 遍历子字符串进行字符串查找
        for(; i + len <= j; i++)
            if(strncmp(&s[i], str, len) == 0)
                // 再将索引值转为位置值，并返回
                return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串右查找函数
int Str_rfind(const char *s, int i, int j, const char *str)
{
    // 声明字符串长度变量
    int len;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 判断查找字符串是否合法
    assert(str);
    // 获取查找字符串长度
    len = strlen(str);
    // 若查找字符串无内容
    if(len == 0)
        // 返回子字符串末尾位置
        return j + 1;
    // 若查找字符串仅含有一个字符
    else if(len == 1)
    {
        // 遍历子字符串进行字符查找，从末尾向前
        while(j > i)
            if(s[--j] == *str)
                // 再将索引值转为位置值，并返回
                return j + 1;
    }
    // 若查找字符串包含多个字符
    else
        // 遍历子字符串进行字符串查找，从末尾向前
        for(;j - len >= i;j--)
            if(strncmp(&s[j - len], str, len) == 0)
                // 再将索引值转为位置值，并返回
                return j - len + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符集合存在函数
int Str_any(const char *s, int i, const char *set)
{
    // 声明字符串长度变量
    int len;

    // 判断字符串是否合法
    assert(s);
    // 判断集合字符串是否合法
    assert(set);
    // 获取字符串长度
    len = strlen(s);
    // 将字符串位置转换为索引
    i = idx(i, len);
    // 判断索引值是否合法
    assert(i >= 0 && i <= len);
    // 若被查找的字符在集合中存在，则返回字符后面的位置值
    if(i < len && strchr(set, s[i]))
        return i + 2;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串头部集合存在函数
int Str_many(const char *s, int i, int j, const char *set)
{
    // 判断集合字符串是否合法
    assert(set);
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 当首字符在集合中时才会向后比较
    if(i < j && strchr(set, s[i]))
    {
        // 向后遍历匹配字符
        do
            i++;
        while(i < j && strchr(set, s[i]));

        // 返回匹配的字符串后面的位置值
        return i + 1;
    }

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串尾部集合存在函数
int Str_rmany(const char *s, int i, int j, const char *set)
{
    // 判断集合字符串是否合法
    assert(set);
    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 当尾字符在集合中时才会向前比较
    if(j > i && strchr(set, s[j - 1]))
    {
        // 向前遍历匹配字符
        do
            --j;
        while(j >= i && strchr(set, s[j]));

        // 返回匹配的字符串前面的位置值
        return j + 2;
    }

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串头部匹配函数
int Str_match(const char *s, int i, int j, const char *str)
{
    // 声明字符串长度变量
    int len;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 判断查找字符串是否合法
    assert(str);
    // 获取查找字符串长度
    len = strlen(str);
    // 若查找字符串无内容
    if(len == 0)
        // 返回子字符串起始位置
        return i + 1;
    // 若查找字符串仅含有一个字符
    else if(len == 1)
    {
        // 若与查找字符串开头匹配
        if(i < j && s[i] == *str)
            // 返回匹配内容后面的位置值
            return i + 2;
    }
    // 若查找字符串包含多个字符
    else
        // 若与查找字符串开头匹配
        if(i + len <= j && strncmp(&s[i], str, len) == 0)
            // 返回匹配内容后面的位置值
            return i + len + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义低级字符串字符串尾部集合存在函数
int Str_rmatch(const char *s, int i, int j, const char *str)
{
    // 声明字符串长度变量
    int len;

    // 将字符串位置转换为可用索引值
    convert(s, i, j);
    // 判断查找字符串是否合法
    assert(str);
    // 获取查找字符串长度
    len = strlen(str);
    // 若查找字符串无内容
    if(len == 0)
        // 返回子字符串末尾位置
        return j + 1;
    // 若查找字符串仅含有一个字符
    else if(len == 1)
    {
        // 若与查找字符串末尾匹配
        if(j > i && s[j - 1] == *str)
            // 返回匹配内容前面的位置值
            return j;
    }
    else
        // 若与查找字符串末尾匹配
        if(j - len >= i && strncmp(&s[j - len], str, len) == 0)
            // 返回匹配内容前面的位置值
            return j - len + 1;

    // 若未查找到则返回零
    return 0;
}

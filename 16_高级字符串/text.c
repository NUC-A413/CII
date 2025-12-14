// 调库
#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "text.h"
#include "mem.h"

#define T Text_T

// 定义字符位置至索引转换宏函数
#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i) - 1)
// 定义字符串空间末尾检查宏函数
#define isatend(s, n) ((s).str+(s).len == current->avail && current->avail + (n) <= current->limit)
// 定义字符串匹配宏函数
#define equal(s, i, t) (memcmp(&(s).str[i], (t).str, (t).len) == 0)

// 定义高级字符串存储结构体
struct Text_save_T
{
    struct chunk *current;
    char *avail;
};

// 定义静态全字符集合，下面的常量字符集合依赖该集合
static char cset[] =
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377";
// 定义全字符集合
const T Text_cset = {256, cset};
// 定义ASCII字符集合
const T Text_ascii = {127, cset};
// 定义大写字母字符集合
const T Text_ucase = {26, cset + 'A'};
// 定义小写字母字符集合
const T Text_lcase = {26, cset + 'a'};
// 定义数字字符集合
const T Text_digits = {10, cset + '0'};
// 定义无字符集合
const T Text_null = {0, cset};

// 定义字符串池结构体，同时定义静态头节点和字符串池指针
static struct chunk
{
    // 该参数指向前置存储块
    struct chunk *link;
    // 该参数指向前置存储块的第一个空闲位置
    char *avail;
    // 该参数指向紧靠前置存储块的末尾的下一内存单元
    char *limit;
} head = {NULL, NULL, NULL}, *current = &head;

// 定义静态内存分配函数
static char *alloc(int len)
{
    // 判断分配大小是否合法
    assert(len >= 0);
    // 若当前存储块内空间不足所需长度
    if(current->avail + len > current->limit)
    {
        // 分配新存储块并移动字符串池指针
        current = current->link = ALLOC(sizeof(*current) + 10 * 1024 + len);
        // 更新字符串池内容
        current->avail = (char *)(current + 1);
        current->limit = current->avail + 10 * 1024 + len;
        current->link = NULL;
    }
    // 留出所需空间
    current->avail += len;

    // 返回所需空间首地址
    return current->avail - len;
}

// 定义高级字符串创建函数
T Text_put(const char *str)
{
    // 声明高级字符串结构体指针
    T text;

    // 判断传入字符串是否合法
    assert(str);
    // 更新高级字符串内容
    text.len = strlen(str);
    text.str = memcpy(alloc(text.len), str, text.len);

    // 返回高级字符串
    return text;
}

// 定义高级字符串获取函数
char *Text_get(char *str, int size, T s)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 判断目标字符串是否为空
    if(str == NULL)
        // 若为空则分配内存空间
        str = ALLOC(s.len + 1);
    else
        // 若不为空则判断内存是否大于高级字符串大小
        assert(size >= s.len + 1);
    // 拷贝字符串至目标字符串
    memcpy(str, s.str, s.len);
    // 最后一位赋值空字符
    str[s.len] = '\0';

    // 返回目标字符串
    return str;
}

// 定义高级字符串打包函数
T Text_box(const char *str, int len)
{
    // 声明高级字符串结构体指针
    T text;

    // 判断传入字符串是否合法
    assert(str);
    // 判断长度是否合法
    assert(len >= 0);
    // 更新高级字符串内容
    text.str = str;
    text.len = len;

    // 返回高级字符串
    return text;
}

// 定义高级字符串子字符串函数
T Text_sub(T s, int i, int j)
{
    // 声明高级字符串结构体指针
    T text;

    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 更新高级字符串内容
    text.len = j - i;
    text.str = s.str + i;

    // 返回高级字符串
    return text;
}

// 定义高级字符串子字符串正位置函数
int Text_pos(T s, int i)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    // 判断索引值是否合法
    assert(i >= 0 && i <= s.len);

    // 再将索引值转为位置值，并返回
    return i + 1;
}

// 定义高级字符串子字符串拼接函数
T Text_cat(T s1, T s2)
{
    // 判读字符串是否合法
    assert(s1.str >= 0 && s1.str);
    assert(s2.str >= 0 && s2.str);
    // 若存在其中一个字符串为空，则直接返回另一个字符串
    if(s1.len == 0)
        return s2;
    if(s2.len == 0)
        return s1;
    // 若两个字符串空间恰好连续
    if(s1.str + s1.len == s2.str)
    {
        // 更新前字符串长度
        s1.len += s2.len;

        // 直接返回前字符串
        return s1;
    }
    {
        // 声明高级字符串结构体指针
        T text;

        // 更新高级字符串内容
        text.len = s1.len + s2.len;
        // 若前字符串在字符串空间的末尾
        if(isatend(s1, s2.len))
        {
            // 将原字符串作为新字符串的起始位置
            text.str = s1.str;
            // 拷贝后字符串至新分配的空间中
            memcpy(alloc(s2.len), s2.str, s2.len);
        }
        else
        {
            // 声明字符指针
            char *p;

            // 为新字符串分配内存空间
            text.str = p = alloc(s1.len + s2.len);
            // 拷贝前后两个字符串至新字符串
            memcpy(p, s1.str, s1.len);
            memcpy(p + s1.len, s2.str, s2.len);
        }

        // 返回高级字符串
        return text;
    }
}

// 定义高级字符串子字符串复制函数
T Text_dup(T s, int n)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 判断复制次数是否合法
    assert(n >= 0);
    // 若复制数量为零，则返回空字符集合
    if(n == 0 || s.len == 0)
        return Text_null;
    // 若复制数量为一，则返回原字符串
    if(n == 1)
        return s;
    // 当复制数量大于一时
    {
        // 声明高级字符串结构体指针
        T text;
        // 声明字符指针
        char *p;

        // 更新高级字符串长度
        text.len = n * s.len;
        // 若原字符串在字符串空间的末尾
        if(isatend(s, text.len - s.len))
        {
            // 将原字符串作为新字符串的第一个备份
            text.str = s.str;
            // 分配剩余字符串备份的内存空间
            p = alloc(text.len - s.len);
            // 重复量减一
            n--;
        }
        // 若原字符串不在字符串空间的末尾
        else
            // 分配新字符串完整的内存空间
            text.str = p = alloc(text.len);
        // 按重复量拷贝原字符串
        for(; n-- > 0; p += s.len)
            memcpy(p, s.str, s.len);

        // 返回高级字符串
        return text;
    }
}

// 定义高级字符串子字符串反转函数
T Text_reverse(T s)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 若字符串长度为零，则返回空字符集合
    if(s.len == 0)
        return Text_null;
    // 若字符串长度为一，则返回原字符串
    else if(s.len == 1)
        return s;
    // 若字符串长度大于一时
    else
    {
        // 声明高级字符串结构体指针
        T text;
        // 声明字符指针
        char *p;
        // 定义计数变量存储原字符串长度
        int i = s.len;

        // 更新高级字符串内容
        text.len = s.len;
        text.str = p = alloc(s.len);
        // 自后向前拷贝字符至新字符串
        while(--i >= 0)
            *p++ = s.str[i];

        // 返回高级字符串
        return text;
    }
}

// 定义高级字符串子字符串映射函数
T Text_map(T s, const T *from, const T *to)
{
    // 声明静态字符映射表，这里映射表长度定为256，匹配所有的256个的字符
    static char map[256];
    // 声明静态字符映射表初始化标志位
    static int inited = 0;

    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 判断映射集合是否存在
    if(from && to)
    {
        // 声明计数变量
        int k;

        // 沿字符映射表遍历，将对应位更新为对应字符
        for(k = 0; k < (int)sizeof map; k++)
            map[k] = k;
        // 判断映射集合长度是否相等
        assert(from->len == to->len);
        // 遍历映射集合，更新字符映射表
        for(k = 0; k < from->len; k++)
            map[from->str[k]] = to->str[k];
        // 更新静态字符映射表初始化标志位
        inited = 1;
    }
    else
    {
        // 判断映射字符串是否为空且原字符串是否存在
        assert(from == NULL && to == NULL);
        // 利用静态字符映射表初始化标志位判断是否为第一次调用该函数
        assert(inited);
    }
    // 若字符串长度为零，则返回空字符集合
    if(s.len == 0)
        return Text_null;
    else
    {
        // 声明高级字符串结构体指针
        T text;
        // 声明计数变量
        int i;
        // 声明字符指针
        char *p;

        // 更新高级字符串内容
        text.len = s.len;
        text.str = p = alloc(s.len);
        // 按映射字符表拷贝字符至新字符串
        for(i = 0; i < s.len; i++)
            *p++ = map[s.str[i]];

        // 返回高级字符串
        return text;
    }
}

// 定义高级字符串子字符串比较函数
int Text_cmp(T s1, T s2)
{
    // 判读字符串是否合法
    assert(s1.len >= 0 && s1.str);
    assert(s2.len >= 0 && s2.str);
    // 若两个字符串位置相同
    if(s1.str == s2.str)
        // 直接返回长度差
        return s1.len - s2.len;
    // 比较字符串长度
    else if(s1.len < s2.len)
    {
        // 调用标准库内存比较函数
        int cond = memcmp(s1.str, s2.str, s1.len);

        // 返回比较结果，这里对字符串长度不同也做了处理
        return cond == 0 ? -1 : cond;
    }
    else if(s1.len > s2.len)
    {
        // 调用标准库内存比较函数
        int cond = memcmp(s1.str, s2.str, s2.len);

        // 返回比较结果，这里对字符串长度不同也做了处理
        return cond == 0 ? +1 : cond;
    }
    else
        // 调用标准库内存比较函数并返回比较结果
        return memcmp(s1.str, s2.str, s1.len);
}

// 定义高级字符串字符左查找函数
int Text_chr(T s, int i, int j, int c)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 遍历字符串进行字符查找
    for(; i < j; i++)
        if(s.str[i] == c)
            // 再将索引值转为位置值，并返回
            return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符右查找函数
int Text_rchr(T s, int i, int j, int c)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 遍历字符串进行字符查找，从末尾向前
    while(j > i)
        if(s.str[--j] == c)
            // 再将索引值转为位置值，并返回
            return j + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符集合左查找函数
int Text_upto(T s, int i, int j, T set)
{
    assert(set.len >= 0 && set.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 遍历字符串进行字符查找
    for(; i < j; i++)
        if(memchr(set.str, s.str[i], set.len))
            // 再将索引值转为位置值，并返回
            return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符集合右查找函数
int Text_rupto(T s, int i, int j, T set)
{
    assert(set.len >= 0 && set.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 遍历字符串进行字符查找，从末尾向前
    while(j > i)
        if(memchr(set.str, s.str[--j], set.len))
            // 再将索引值转为位置值，并返回
            return j + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符集合存在函数
int Text_any(T s, int i, T set)
{
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    assert(set.len >= 0 && set.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    // 判断索引值是否合法
    assert(i >= 0 && i <= s.len);
    // 若被查找的字符在集合中存在，则返回字符后面的位置值
    if(i < s.len && memchr(set.str, s.str[i], set.len))
        return i + 2;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符串头部集合存在函数
int Text_many(T s, int i, int j, T set)
{
    assert(set.len >= 0 && set.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 当首字符在集合中时才会向后比较
    if(i < j && memchr(set.str, s.str[i], set.len))
    {
        // 向后遍历匹配字符
        do
            i++;
        while(i < j && memchr(set.str, s.str[i], set.len));

        // 返回匹配的字符串后面的位置值
        return i + 1;
    }

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符串尾部集合存在函数
int Text_rmany(T s, int i, int j, T set)
{
    assert(set.len >= 0 && set.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 当尾字符在集合中时才会向前比较
    if(j > i && memchr(set.str, s.str[j - 1], set.len))
    {
        // 向前遍历匹配字符
        do
            --j;
        while(j >= i && memchr(set.str, s.str[j], set.len));

        // 返回匹配的字符串前面的位置值
        return j + 2;
    }

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符串左查找函数
int Text_find(T s, int i, int j, T str)
{
    assert(str.len >= 0 && str.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 若字符串长度为零
    if(str.len == 0)
        // 返回子字符串起始位置
        return i + 1;
    // 若字符串长度为一
    else if(str.len == 1)
    {
        // 遍历字符串进行字符查找
        for(; i < j; i++)
            if(s.str[i] == *str.str)
                // 再将索引值转为位置值，并返回
                return i + 1;
    }
    // 若字符串长度大于一时
    else
        // 遍历字符串进行字符串查找
        for(; i + str.len <= j; i++)
            // 调用字符串匹配宏函数
            if(equal(s, i, str))
                // 再将索引值转为位置值，并返回
                return i + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符串右查找函数
int Text_rfind(T s, int i, int j, T str)
{
    assert(str.len >= 0 && str.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 若字符串长度为零
    if(str.len == 0)
        // 返回子字符串起始位置
        return j + 1;
    // 若字符串长度为一
    else if(str.len == 1)
    {
        while(j > i)
            if(s.str[--j] == *str.str)
                // 再将索引值转为位置值，并返回
                return j + 1;
    }
    // 若字符串长度大于一时
    else
        // 遍历字符串进行字符串查找，从末尾向前
        for(; j - str.len >= i; j--)
            // 调用字符串匹配宏函数
            if(equal(s, j - str.len, str))
                // 再将索引值转为位置值，并返回
                return j - str.len + 1;

    // 若未查找到则返回零
    return 0;
}

// 定义高级字符串字符串头部匹配函数
int Text_match(T s, int i, int j, T str)
{
    assert(str.len >= 0 && str.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 若字符串长度为零
    if(str.len == 0)
        return i + 1;
    // 若字符串长度为一
    else if(str.len == 1)
    {
        // 若与查找字符串开头匹配
        if(i < j && s.str[i] == *str.str)
            // 返回匹配内容后面的位置值
            return i + 2;
    }
    // 若字符串长度大于一时
    else
        // 若与查找字符串开头匹配
        if(i + str.len <= j && equal(s, i, str))
            // 返回匹配内容后面的位置值
            return i + str.len + 1;

    return 0;
}

// 定义高级字符串字符串尾部匹配函数
int Text_rmatch(T s, int i, int j, T str)
{
    assert(str.len >= 0 && str.str);
    // 判读字符串是否合法
    assert(s.len >= 0 && s.str);
    // 将字符串位置转换为索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    // 若前后索引大小相仿，则交换
    if(i > j)
    {
        int t = i;
        i = j;
        j = t;
    }
    // 判断索引值是否合法
    assert(i >= 0 && j <= s.len);
    // 若字符串长度为零
    if(str.len == 0)
        // 返回子字符串末尾位置
        return j + 1;
    // 若字符串长度为一
    else if(str.len == 1)
    {
        // 若与查找字符串末尾匹配
        if(j > i && s.str[j - 1] == *str.str)
            // 返回匹配内容前面的位置值
            return j;
    }
    // 若字符串长度大于一时
    else
        // 若与查找字符串末尾匹配
        if(j - str.len >= i && equal(s, j - str.len, str))
            // 返回匹配内容前面的位置值
            return j - str.len + 1;

    return 0;
}

// 定义高级字符串格式化函数
void Text_fmt(int code, va_list *app, int put(int c, void *cl), void *cl, unsigned char flags[], int width, int precision)
{
    // 声明高级字符串结构体二级指针
    T *s;

    // 判断可变参数列表指针和格式化标志是否合法
    assert(app && flags);
    // 获取可变参数列表中的高级字符串数据
    s = va_arg(*app, T *);
    // 判断该高级字符串是否合法
    assert(s && s->len >= 0 && s->str);
    // 调用格式化字符串传递函数输出高级字符串
    Fmt_puts(s->str, s->len, put, cl, flags, width, precision);
}

// 定义高级字符串存储函数
Text_save_T Text_save(void)
{
    // 声明高级字符串存储结构体指针
    Text_save_T save;

    // 分配内存空间
    NEW(save);
    // 更新存储结构体内容，保存当前字符串池尾部
    save->current = current;
    save->avail = current->avail;
    // 创建一个字符空洞
    alloc(1);

    // 返回高级字符串存储结构体指针
    return save;
}

// 定义高级字符串恢复函数
void Text_restore(Text_save_T *save)
{
    // 声明字符串池结构体指针
    struct chunk *p, *q;

    // 判断高级字符串存储结构体二级指针和指针是否合法
    assert(save && *save);
    // 将字符串池尾部还原
    current = (*save)->current;
    current->avail = (*save)->avail;
    // 释放高级字符串存储结构体
    FREE(*save);
    // 将当前存储块后所有存储块都释放
    for(p = current->link; p; p = q)
    {
        q = p->link;
        FREE(p);
    }
    // 更新当前存储块内容
    current->link = NULL;
}

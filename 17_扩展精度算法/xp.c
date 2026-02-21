// 调库
#include <ctype.h>
#include <string.h>
#include "assert.h"
#include "xp.h"

#define T XP_T

// 定义底数，取256作为底数可将完整数字转换为用字节存储的形式
#define BASE (1<<8)

// 定义字符转数字映射表
static char map[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    36, 36, 36, 36, 36, 36, 36,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 36, 36, 36, 36, 36,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

// 定义进位加法函数
int XP_add(int n, T z, T x, T y, int carry)
{
    // 声明计数变量
    int i;

    // 按位依次进行求和
    for(i = 0; i < n; i++)
    {
        // 将两数和进位值相加，保存在进位项中
        carry += x[i] + y[i];
        // 将前面相加结果对底数取余，得到当前位结果
        z[i] = carry % BASE;
        // 将相加结果除以底数，得到进位值
        carry /= BASE;
        // 这里相加结果先保存在进位项中，要是先保存在结果数组中，会造成溢出
    }

    // 返回最终进位值
    return carry;
}

// 定义借位减法函数
int XP_sub(int n, T z, T x, T y, int borrow)
{
    // 声明计数变量
    int i;

    // 按位依次进行求差
    for(i = 0; i < n; i++)
    {
        // 计算当前位，被减数加底数减去借位值再减减数
        int d = (x[i] + BASE) - borrow - y[i];
        // 将相减结果对底数取余，得到当前位结果
        z[i] = d % BASE;
        // 计算当前位下的借位值
        // 若实际计算时需要借位，则相减结果一定小于底数，若无需借位，则一定大于等于底数
        borrow = 1 - d / BASE;
    }

    // 返回最终借位值
    return borrow;
}

// 定义乘法函数
int XP_mul(T z, int n, T x, int m, T y)
{
    // 声明计数变量并定义进位输出项
    int i, j, carryout = 0;

    // 遍历x各个位
    for(i = 0; i < n; i++)
    {
        // 定义进位项
        unsigned carry = 0;
        // 使用x当前位遍历相乘y各个位
        for(j = 0; j < m; j++)
        {
            // 将两数乘积、当前位先前值和进位值相加，保存在进位项中
            carry += x[i] * y[j] + z[i + j];
            // 将上面计算结果除以底数，得到当前位结果
            z[i + j] = carry % BASE;
            // 将上面计算结果除以底数，得到进位值
            carry /= BASE;
        }
        // 对更高位进行进位修正
        for(; j < n + m - i; j++)
        {
            // 将当前位先前值和进位值相加，保存在进位项中
            carry += z[i + j];
            // 将上面计算结果除以底数，得到当前位结果
            z[i + j] = carry % BASE;
            // 将上面计算结果除以底数，得到进位值
            carry /= BASE;
        }
        // 记录每次循环可能的进位，虽然理论不存在，属于防御性编程措施
        carryout |= carry;
    }

    // 返回最终进位值
    return carryout;
}

// 定义除法函数
int XP_div(int n, T q, T x, int m, T y, T r, T tmp)
{
    // 存储被除数和除数位数
    int nx = n, my = m;

    // 重新获取被除数和除数位数
    n = XP_length(n, x);
    m = XP_length(m, y);
    // 若除数位数为1
    if(m == 1)
    {
        // 判断除数是否为0
        if(y[0] == 0)
            return 0;
        // 调用一位除法函数
        r[0] = XP_quotient(nx, q, x, y[0]);
        // 将余数后面字节清空
        memset(r + 1, '\0', my - 1);
    }
    // 若除数位数大于被除数
    else if(m > n)
    {
        // 商为0
        memset(q, '\0', nx);
        // 余数等于被除数
        memcpy(r, x, n);
        // 将余数后面字节清空
        memset(r + n, '\0', my - n);
    }
    else
    {
        // 声明计数变量
        int k;
        // 使用临时空间定义临时变量被除数余量和被除数差值
        unsigned char *rem = tmp, *dq = tmp + n + 1;

        // 判断除数和被除数位数是否合法
        assert(2 <= m && m <= n);
        // 将被除数拷贝至被除数余量
        memcpy(rem, x, n);
        // 将被除数余量最高位设为0
        rem[n] = 0;
        // 从高位依次向下求商
        for(k = n - m; k >= 0; k--)
        {
            // 声明商的当前位
            int qk;

            // 计算商的当前位，求出被除数差值
            {
                // 声明计数变量
                int i;

                // 判断除数位数和当前位是否合法
                assert(2 <= m && m <= k + m && k + m <= n);
                // 求商的当前位
                {
                    // 定义当前位项
                    int km = k + m;
                    // 取除数前两位
                    unsigned long y2 = y[m - 1] * BASE + y[m - 2];
                    // 取被除数余量的前三位
                    unsigned long r3 = rem[km] * (BASE * BASE) + rem[km - 1] * BASE + rem[km - 2];

                    // 使用前面两值计算商的当前位
                    qk = r3 / y2;
                    // 对商值进行约束
                    if(qk >= BASE)
                        qk = BASE - 1;
                }
                // 调用一位乘法函数求出被除数差值
                dq[m] = XP_product(m, dq, y, qk);
                // 从高位向下判断被除数余量和被除数差值前几位是否相同，更新计数变量
                for(i = m; i > 0; i--)
                    if(rem[i + k] != dq[i])
                        break;
                // 当有位的值不同时，判断是否是被除数余量该位小于被除数差值该位
                if(rem[i + k] < dq[i])
                    // 若为是，则递减商的当前位，并更新被除数差值
                    dq[m] = XP_product(m, dq, y, --qk);
            }
            // 保存商的当前位
            q[k] = qk;
            // 更新被除数余量
            {
                // 声明借位项
                int borrow;

                // 判断计算位是否合法
                assert(0 <= k && k <= k + m);
                // 调用减法函数
                borrow = XP_sub(m + 1, &rem[k], &rem[k], dq, 0);
                // 判断借位项是否为0
                assert(borrow == 0);
            }
        }
        // 将最终的被除数余量拷贝至余数
        memcpy(r, rem, m);
        // 填充商和余数的高位
        {
            // 声明计数变量
            int i;

            // 将商后面字节清空
            for(i = n - m + 1; i < nx; i++)
                q[i] = 0;
            // 将余数后面字节清空
            for(i = m; i < my; i++)
                r[i] = 0;
        }
    }

    // 返回执行成功
    return 1;
}

// 定义一位加法函数
int XP_sum(int n, T z, T x, int y)
{
    // 声明计数变量
    int i;

    // 按位依次进行求和
    for(i = 0; i < n; i++)
    {
        // 将加数和进位值相加，保存在进位项中
        y += x[i];
        // 将前面相加结果对底数取余，得到当前位结果
        z[i] = y % BASE;
        // 将相加结果除以底数，得到进位值
        y /= BASE;
    }

    // 返回最终进位值
    return y;
}

// 定义一位减法函数
int XP_diff(int n, T z, T x, int y)
{
    // 声明计数变量
    int i;

    // 按位依次进行求差
    for(i = 0; i < n; i++)
    {
        // 计算当前位，被减数加底数减去借位值
        int d = (x[i] + BASE) - y;
        // 将相减结果对底数取余，得到当前位结果
        z[i] = d % BASE;
        // 计算当前位下的借位值
        y = 1 - d / BASE;
    }

    // 返回最终借位值
    return y;
}

// 定义一位乘法函数
int XP_product(int n, T z, T x, int y)
{
    // 声明计数变量
    int i;
    // 定义进位项
    unsigned carry = 0;

    // 按位依次进行求积
    for(i = 0; i < n; i++)
    {
        // 将两数乘积和进位值相加，保存在进位项中
        carry += x[i] * y;
        // 将上面计算结果除以底数，得到当前位结果
        z[i] = carry % BASE;
        // 将上面计算结果除以底数，得到进位值
        carry /= BASE;
    }

    // 返回最终进位值
    return carry;
}

// 定义一位除法函数
int XP_quotient(int n, T z, T x, int y)
{
    // 声明计数变量
    int i;
    // 定义余数项
    unsigned carry = 0;

    // 从高位依次向下求商
    for(i = n - 1; i >= 0; i--)
    {
        // 将余数代入更新当前位
        carry = carry * BASE + x[i];
        // 将上面计算结果除以除数，得到当前位结果
        z[i] = carry / y;
        // 将上面计算结果对底数取余，得到余数
        carry %= y;
    }

    // 返回最终余数值
    return carry;
}

// 定义取反取补函数
int XP_neg(int n, T z, T x, int carry)
{
    // 声明计数变量
    int i;

    // 按位依次进行取反并运算
    for(i = 0; i < n; i++)
    {
        // 计算当前位，将原数该位取反并加上进位值
        carry += (unsigned char)~x[i];
        // 将前面相加结果对底数取余，得到当前位结果
        z[i] = carry % BASE;
        // 将相加结果除以底数，得到进位值
        carry /= BASE;
    }

    // 返回最终进位值
    return carry;
}

// 定义比较函数
int XP_cmp(int n, T x, T y)
{
    // 定义计数变量
    int i = n - 1;

    // 从高位依次向下比较
    while(i > 0 && x[i] == y[i])
        i--;

    // 返回比较结果
    return x[i] - y[i];
}

// 定义左移函数
void XP_lshift(int n, T z, int m, T x, int s, int fill)
{
    // 更新填充字节
    fill = fill ? 0xFF : 0;
    // 按字节左移
    {
        // 声明并定义计数变量
        int i, j = n - 1;

        // 若保存空间大于要移动的数字的空间
        if(n > m)
            // 更新计数变量对齐数字最高位
            i = m - 1;
        // 若小于
        else
            // 则更新计数变量对齐数字移动后有效的最高位
            i = n - s / 8 - 1;
        // 对移动后高位填充为0
        for(; j >= m + s / 8; j--)
            z[j] = 0;
        // 从高位依次向下存储
        for(; i >= 0; i--, j--)
            z[j] = x[i];
        // 对移动后低位进行填充
        for(; j >= 0; j--)
            z[j] = fill;
    }
    // 获取移动位数
    s %= 8;
    // 按位左移
    if(s > 0)
    {
        // 调用一位乘法函数，利用乘法进行位移
        XP_product(n, z, z, 1 << s);
        // 对移动后低位进行填充
        z[0] |= fill >> (8 - s);
    }
}

// 定义右移函数
void XP_rshift(int n, T z, int m, T x, int s, int fill)
{
    // 更新填充字节
    fill = fill ? 0xFF : 0;
    // 按字节右移
    {
        // 声明并定义计数变量
        int i, j = 0;

        // 从低位依次向上存储
        for(i = s / 8; i < m && j < n; i++, j++)
            z[j] = x[i];
        // 对移动后高位进行填充
        for(; j < n; j++)
            z[j] = fill;
    }
    // 获取移动位数
    s %= 8;
    // 按位右移
    if(s > 0)
    {
        // 调用一位除法函数，利用除法进行位移
        XP_quotient(n, z, z, 1 << s);
        // 对移动后高位进行填充
        z[n - 1] |= fill << (8 - s);
    }
}

// 定义数字位数函数
int XP_length(int n, T x)
{
    // 计算有效数字个数，忽略引导0的数量
    while(n > 1 && x[n - 1] == 0)
        n--;

    // 返回有效数字个数
    return n;
}

// 定义整型获取函数
unsigned long XP_fromint(int n, T z, unsigned long u)
{
    // 定义计数变量
    int i = 0;

    // 以256为底数依次保存各位
    do
        // 对底数取余
        z[i++] = u % BASE;
    // 当被除数仍存在且位数未超时继续保存
    while((u /= BASE) > 0 && i < n);
    // 将后续未使用位清零
    for(; i < n; i++)
        z[i] = 0;

    // 返回原数字
    return u;
}

// 定义整型表示函数
unsigned long XP_toint(int n, T x)
{
    // 声明存储变量
    unsigned long u = 0;
    // 定义计数变量
    int i = (int)sizeof u;

    // 当有效位大于扩展精度数据格式下位数时，更新计数值
    if(i > n)
        i = n;
    // 将扩展精度数据格式还原，仅保留无符号长整形长度的最低有效位
    while(--i >= 0)
        u = BASE * u + x[i];

    // 返回转换后数字
    return u;
}

// 定义字符串获取函数
int XP_fromstr(int n, T z, const char *str, int base, char **end)
{
    // 定义字符指针记录字符串首字符地址
    const char *p = str;

    // 判断上面字符串指针常量是否合法
    assert(p);
    // 判断底数是否超出范围
    assert(base >= 2 && base <= 36);
    // 忽略字符串最前面的空格
    while(*p && isspace(*p))
        p++;
    // 判断当前字符是否存在，且是否为字符数字，且是否超过底数
    if(*p && isalnum(*p) && map[*p - '0'] < base)
    {
        // 声明进位项
        int carry;

        // 依次获取各个位数字
        for(; *p && isalnum(*p) && map[*p - '0'] < base; p++)
        {
            // 调用一位乘法函数，将已获取的数字乘以底数
            carry = XP_product(n, z, z, base);
            // 若位数不够，则停止获取数字
            if(carry)
                break;
            // 调用一位加法函数，获取当前位数字
            XP_sum(n, z, z, map[*p - '0']);
        }
        // 判断结束字符串指针是否存在，保存后续字符串
        if(end)
            *end = (char *)p;

        // 返回最终进位值
        return carry;
    }
    // 若字符不符合提取要求
    else
    {
        // 判断结束字符串指针是否存在，保存完整字符串
        if(end)
            *end = (char *)str;

        // 返回失败标志
        return 0;
    }
}

// 定义字符串表示函数
char *XP_tostr(char *str, int size, int base, int n, T x)
{
    // 定义计数变量
    int i = 0;

    // 判断字符串是否合法
    assert(str);
    // 判断底数是否超出范围
    assert(base >= 2 && base <= 36);
    // 将数字转换为字符串
    do
    {
        // 调用一位除法函数，依次获取数字各个位
        int r = XP_quotient(n, x, x, base);

        // 判断计数变量是否超过字符串长度
        assert(i < size);
        // 将当前位转换为字符串
        str[i++] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[r];
        // 清除高位的空位
        while(n > 1 && x[n - 1] == 0)
            n--;
    } while(n > 1 || x[0] != 0);
    // 判断计数变量是否超过字符串长度
    assert(i < size);
    // 将字符串末尾更新为空字符
    str[i] = '\0';
    // 调转字符串
    // 在进制转换中，越早得到的余数，位数越小，但字符串表示相反，则需要调换下字符串方向
    {
        // 声明计数变量
        int j;

        // 头尾依次调换
        for(j = 0; j < --i; j++)
        {
            char c = str[j];
            str[j] = str[i];
            str[i] = c;
        }
    }

    // 返回最终字符串
    return str;
}

#ifndef BIT_INCLUDED
#define BIT_INCLUDED

// 库内简化写法
#define T Bit_T

// 声明位向量结构体隐性指针
typedef struct T *T;

// 声明位向量创建函数
extern T Bit_new(int length);
// 声明位向量释放函数
extern void Bit_free(T *set);
// 声明位向量计长函数
extern int Bit_length(T set);
// 声明位向量计数函数
extern int Bit_count(T set);
// 声明位向量查找函数
extern int Bit_get(T set, int n);
// 声明位向量添加函数
extern int Bit_put(T set, int n, int bit);
// 声明位向量批量清除函数
extern void Bit_clear(T set, int lo, int hi);
// 声明位向量批量设置函数
extern void Bit_set(T set, int lo, int hi);
// 声明位向量翻转函数
extern void Bit_not(T set, int lo, int hi);
// 声明位向量子集函数
extern int Bit_leq(T s, T t);
// 声明位向量真子集函数
extern int Bit_lt(T s, T t);
// 声明位向量相等函数
extern int Bit_eq(T s, T t);
// 声明位向量遍历调用函数
extern void Bit_map(T set, void apply(int n, int bit, void *cl), void *cl);
// 声明位向量并集函数
extern T Bit_union(T s, T t);
// 声明位向量交集函数
extern T Bit_inter(T s, T t);
// 声明位向量差集函数
extern T Bit_minus(T s, T t);
// 声明位向量对称差集函数
extern T Bit_diff(T s, T t);

#undef T
#endif

#ifndef SET_INCLUDED
#define SET_INCLUDED

// 库内简化写法
#define T Set_T

// 声明集合结构体隐性指针
typedef struct T *T;

// 声明集合创建函数
extern T Set_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *x));
// 声明集合释放函数
extern void Set_free(T *set);
// 声明集合计数函数
extern int Set_length(T set);
// 声明集合查找函数
extern int Set_member(T set, const void *member);
// 声明集合添加函数
extern void Set_put(T set, const void *member);
// 声明集合删除函数
extern void *Set_remove(T set, const void *member);
// 声明集合并集函数
extern T Set_union(T s, T t);
// 声明集合交集函数
extern T Set_inter(T s, T t);
// 声明集合差集函数
extern T Set_minus(T s, T t);
// 声明集合对称差集函数
extern T Set_diff(T s, T t);
// 声明集合遍历调用函数
extern void Set_map(T set, void apply(const void *member, void *cl), void *cl);
// 声明集合转数组函数
extern void **Set_toArray(T set, void *end);

#undef T
#endif

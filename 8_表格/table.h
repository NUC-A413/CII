#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED

// 库内简化写法
#define T Table_T

// 声明表格结构体隐性指针
typedef struct T *T;

// 声明表格创建函数
extern T Table_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *key));
// 声明表格释放函数
extern void Table_free(T *table);
// 声明表格计数函数
extern int Table_length(T table);
// 声明表格添加函数
extern void *Table_put(T table, const void *key, void *value);
// 声明表格查找函数
extern void *Table_get(T table, const void *key);
// 声明表格删除函数
extern void *Table_remove(T table, const void *key);
// 声明表格遍历调用函数
extern void Table_map(T table, void apply(const void *key, void **value, void *cl), void *cl);
// 声明表格转数组函数
extern void **Table_toArray(T table, void *end);

#undef T
#endif

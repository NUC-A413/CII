#ifndef ARRAY_INCLUDED
#define ARRAY_INCLUDED

// 库内简化写法
#define T Array_T

// 声明动态数组结构体隐性指针
typedef struct T *T;

// 声明动态数组创建函数
extern T Array_new(int length, int size);
// 声明动态数组释放函数
extern void Array_free(T *array);
// 声明动态数组计数函数
extern int Array_length(T array);
// 声明动态数组计大小函数
extern int Array_size(T array);
// 声明动态数组查找函数
extern void *Array_get(T array, int i);
// 声明动态数组添加函数
extern void *Array_put(T array, int i, void *elem);
// 声明动态数组重置大小函数
extern void Array_resize(T array, int length);
// 声明动态数组拷贝函数
extern T Array_copy(T array, int length);

#undef T
#endif

#ifndef RING_INCLUDED
#define RING_INCLUDED

// 库内简化写法
#define T Ring_T

// 声明环结构体隐性指针
typedef struct T *T;

// 声明环新建函数
extern T Ring_new(void);
// 声明环创建函数
extern T Ring_ring(void *x, ...);
// 声明环释放函数
extern void Ring_free(T *ring);
// 声明环计数函数
extern int Ring_length(T ring);
// 声明环查找函数
extern void *Ring_get(T ring, int i);
// 声明环修改函数
extern void *Ring_put(T ring, int i, void *x);
// 声明环添加函数
extern void *Ring_add(T ring, int pos, void *x);
// 声明环删除函数
extern void *Ring_remove(T ring, int i);
// 声明环低端添加函数
extern void *Ring_addlo(T ring, void *x);
// 声明环高端添加函数
extern void *Ring_addhi(T ring, void *x);
// 声明环低端删除函数
extern void *Ring_remlo(T ring);
// 声明环高端删除函数
extern void *Ring_remhi(T ring);
// 声明环旋转函数
extern void *Ring_rotate(T ring, int n);

#undef T
#endif

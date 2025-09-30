#ifndef LIST_INCLUDED
#define LIST_INCLUDED

// 库内简化写法
#define T List_T

// 声明链表节点结构体隐性指针
typedef struct T *T;

// 定义链表节点结构体
struct T
{
    struct T *rest;
    void *first;
};

// 声明链表追加函数
extern T List_append(T list, T tail);
// 声明链表拷贝函数
extern T List_copy(T list);
// 声明链表创建函数
extern T List_list(void *x, ...);
// 声明链表弹栈函数
extern T List_pop(T list, void **x);
// 声明链表压栈函数
extern T List_push(T list, void *x);
// 声明链表反转函数
extern T List_reverse(T list);
// 声明链表计数函数
extern int List_length(T list);
// 声明链表释放函数
extern void List_free(T *list);
// 声明链表遍历调用函数
extern void List_map(T list, void apply(void **x, void *cl), void *cl);
// 声明链表转数组函数
extern void **List_toArray(T list, void *end);

#undef T
#endif

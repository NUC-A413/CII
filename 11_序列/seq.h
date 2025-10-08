#ifndef SEQ_INCLUDED
#define SEQ_INCLUDED

// 库内简化写法
#define T Seq_T

// 声明序列结构体隐性指针
typedef struct T *T;

// 声明序列新建函数
extern T Seq_new(int hint);
// 声明序列创建函数
extern T Seq_seq(void *x, ...);
// 声明序列释放函数
extern void Seq_free(T *seq);
// 声明序列计数函数
extern int Seq_length(T seq);
// 声明序列查找函数
extern void *Seq_get(T seq, int i);
// 声明序列添加函数
extern void *Seq_put(T seq, int i, void *x);
// 声明序列低端添加函数
extern void *Seq_addlo(T seq, void *x);
// 声明序列高端添加函数
extern void *Seq_addhi(T seq, void *x);
// 声明序列低端删除函数
extern void *Seq_remlo(T seq);
// 声明序列高端删除函数
extern void *Seq_remhi(T seq);

#undef T
#endif

#ifndef ARRAYREP_INCLUDED
#define ARRAYREP_INCLUDED

// 库内简化写法
#define T Array_T

// 定义动态数组结构体
struct T
{
    // 该参数为成员数量
    int length;
    // 该参数为成员大小
    int size;
    // 该参数是数组
    char *array;
};

// 声明动态数组结构体初始化函数
// 这里采用分层接口设计，为了进行权限的隔离，同时降低接口间耦合程度
extern void ArrayRep_init(T array, int length, int size, void *ary);

#undef T
#endif

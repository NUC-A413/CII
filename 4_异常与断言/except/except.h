#ifndef RXCEPT_INCLUDED
#define EXCEPT_INCLUDED

// 调用标准库，该库用于非本地跳转，在错误处理和异常处理中非常有用，但存在潜在问题
#include <setjmp.h>

// 库内简化写法
#define T Except_T

// 声明异常结构体
typedef struct T
{
    char *reason;
} T;

// 声明异常栈帧结构体
struct Except_Frame
{
    // 前置节点
    Except_Frame *prev;
    // 跳转环境位置
    jmp_buf env;
    // 文件
    const char *file;
    // 行数
    int line;
    // 异常
    const T *exception;
}

// 简化写法
typedef struct Except_Frame Except_Frame;
// 声明异常处理栈栈顶指针
extern Except_Frame *Except_stack;
// 声明断言异常
extern const Except_T Assert_Failed;

// 声明异常处理状态
enum
{
    Except_entered = 0,
    Except_raised,
    Except_handled,
    Except_finalized
};

// 声明异常提出函数
void Except_raise(const T *e, const char *file, int line);

// 简化异常抛出
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
// 定义异常重新抛出
#define RERAISE Except_raise(Except_frame.exception, Except_frame.file, Except_frame.line)
// 定义异常处理中函数返回，这里炫技操作，先执行弹栈操作，将0返回给switch，永远执行default后的行为
#define RETURN switch(Except_stack = Except_stack->prev,0) default: return
// 定义测试关键字
#define TRY \
    do{ \
        volatile int Except_flag; /* 声明异常处理状态标记 */ \
        Except_Frame Except_frame; /* 声明异常处理新栈帧 */ \
        Except_frame.prev = Except_stack; /* 这两句是压栈 */ \
        Except_stack = &Except_frame; \
        Except_flag = setjmp(Except_frame.env); /* 记录跳转位置环境，更新状态标记至异常处理接收状态 */ \
        if(Except_flag == Except_entered){ /* 判断是否在异常处理接收状态 */
// 定义处理异常关键字
#define EXCEPT(e) \
            if(Except_flag == Except_entered) /* 如果测试语句未出现异常 */ \
                Except_stack = Except_stack->prev;} /* 弹栈 */ \
        else if(Except_frame.exception == &(e)){ /* 如果测试语句异常与提供异常相同 */ \
            Except_flag = Except_handled; /* 更新状态标记至异常处理执行状态 */
// 定义处理其他异常关键字
#define ELSE \
            if(Except_flag == Except_entered) /* 如果测试语句未出现异常 */ \
                Except_stack = Except_stack->prev;} /* 弹栈 */ \
        else{ /* 如果测试语句异常是未提供的异常*/ \
            Except_flag = Except_handled; /* 更新状态标记至异常处理执行状态*/
// 定义均执行关键字，无论是否有异常抛出，都执行该代码块下内容
#define FINALLY \
            if(Except_flag == Except_entered) /* 如果测试语句未出现异常 */ \
                Except_stack = Except_stack->prev;}{ /* 弹栈 */ \
            if(Except_flag == Except_entered) /* 如果测试语句未出现异常 */ \
                Except_flag = Except_finalized; /* 更新状态标记至异常处理终止状态 */
// 定义终止测试关键字
#define END_TRY \
            if(Except_flag == Except_entered) /* 如果测试语句未出现异常 */ \
                Except_stack = Except_stack->prev;} /* 弹栈 */ \
        if(Except_flag == Except_raised) RERAISE; /* 当状态标记仍是异常抛出状态时，跳转回测试语句前，重新抛出异常*/ \
    } while(0)
// TIP：整个异常处理过程都在do{...}while(0)零次循环中实现，这是C语言编程中一个非常常用且无副作用的操作
// 它的主要用途有四种：
// 1. 避免宏展开错误：如果直接用{}包裹宏内容，可能因多余分号和语法冲突导致编译错误
// 2. 可以使用break语句跳出
// 3. 定义局部作用域：在内部定义的变量仅在循环作用域内有效，避免与外界变量发生冲突
// 4. 避免空宏警告：部分编译器下对空宏定义会警告，可写作#define NO_OP do{}while(0) 来避免

#undef T
#endif

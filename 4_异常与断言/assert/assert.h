// 取消宏定义，避免宏定义冲突
#undef assert

// 非调试模式下不启用断言
#ifdef NDEBUG

#define assert(e) ((void)0)

#else

// 调用异常库
#include "except.h"

// 声明断言函数
extern void assert(int e);

// 定义断言宏
// 这里用到了||和,运算符的特性，当e条件为真时，||运算符后面的语句不会再执行
// 当e条件为假时，执行异常抛出语句，同时用,运算符确保||运算符后语句永为假
#define assert(e) ((void)((e)||(RAISE(Assert_Failed),0)))

#endif

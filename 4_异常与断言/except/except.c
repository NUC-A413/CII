#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "except.h"
#define T Except_T

// 初始化异常处理栈栈顶指针
Except_Frame *Except_stack = NULL;

void Except_raise(const T *e, const char *file, int line)
{
    // 拉取异常处理栈
    Except_Frame *p = Except_stack;

    // 判断异常指针是否合法
    assert(e);
    // 判断异常处理栈是否为空，若为空，说明当前异常出现并未在测试框架内
    if(p == NULL)
    {
        // 未捕捉的异常
        // fprintf函数用于将格式化输出发送到指定的文件流中
        // stderr是标准错误输出流，是计算机系统中用于输出错误信息的默认通道，默认输出到终端，无缓冲机制
        fprintf(stderr, "Uncaught exception");
        // 打印原因或异常地址
        if(e->reason)
            fprintf(stderr, " %s", e->reason);
        else
            fprintf(stderr, " at 0x%p", e);
        // 打印文件名和行号
        if(file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);
        // 异常处理中止
        fprintf(stderr, "aborting...\n");
        // 将标准错误输出流清空
        fflush(stderr);
        // 中止程序
        abort();
    }
    // 更新异常处理栈栈帧内异常
    p->exception = e;
    // 更新异常处理栈栈帧内文件名
    p->file = file;
    // 更新异常处理栈栈帧内行号
    p->line = line;
    // 弹栈
    Except_stack = Except_stack->prev;
    // 跳转回测试语句前，更新状态标志为异常抛出状态
    longjmp(p->env, Except_raised);
}

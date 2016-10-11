/*******************************************************************************
* 文 件 名: cpulib_def.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: CPU公共库的宏定义
*******************************************************************************/

#ifndef __CPULIB_DEF_H
#define __CPULIB_DEF_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpu_port.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* 结构体宏 ------------------------------------------------------------------*/
/*获取数组大小*/
#define ARRAY_SIZE(arr)             ( sizeof(arr)/sizeof(*(arr)) )

/*获取结构体成员变量的偏移量*/
#ifndef offsetof
    #define offsetof(type,member)   ( (size_t)(&((type *)0)->member) )
#endif

/*通过结构体成员变量指针获得结构体指针*/
#ifndef container_of
    #define container_of(ptr, type, member) \
        ( (type *)((char *)(ptr) - offsetof(type, member)) )
#endif

/* 调试宏 --------------------------------------------------------------------*/
#ifndef NDEBUG
    #define debug_assert(expr)      CPU_Assert(expr)
    #define debug_coverage()        CPU_Coverage()
    #define debug_printf(...)       CPU_Printf(__VA_ARGS__)
#else   /* NDEBUG */
    #define debug_assert(expr)      ((void)0)
    #define debug_coverage()        ((void)0)
    #define debug_printf(...)       ((void)0)
#endif  /* NDEBUG */

#endif  /* __CPULIB_DEF_H */

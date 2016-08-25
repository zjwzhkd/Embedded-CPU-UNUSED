/*******************************************************************************
* MCU型 号: STM32F1XX
* 文 件 名: cpu_port.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: CPU接口定义
*******************************************************************************/

#ifndef __CPU_PORT_H
#define __CPU_PORT_H

/* 头文件 --------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "cpu_config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* 数据类型 ------------------------------------------------------------------*/
/*CPU体系数据类型*/
typedef int32_t         base_t;
typedef uint32_t        ubase_t;
typedef base_t          cpu_t;

/*节拍类型*/
#ifdef CPU_USE_16BIT_TICK
    typedef uint16_t    tick_t;
#else
    typedef uint32_t    tick_t;
#endif

/* 编译器宏 ------------------------------------------------------------------*/
/*声明数据保存在FLASH上*/
#define FLASH_DATA
/*声明数据保存在EEPROM上*/
#define EEPROM_DATA
/*静态内联函数*/
#ifndef STATIC_INLINE
    #define STATIC_INLINE static inline
#endif

/* CPU中断管理 ---------------------------------------------------------------*/
/*判断CPU是否处于处理模式*/
#define cpu_InHandlerMode()     ( 0 != __get_IPSR() )

#define cpu_irq_enable()        __enable_irq()
#define cpu_irq_disable()       __disable_irq()

STATIC_INLINE cpu_t cpu_irq_save(void)
{
cpu_t cpu_sr;

    cpu_sr = __get_PRIMASK();
    cpu_irq_disable();
    return (cpu_sr);
}
STATIC_INLINE void cpu_irq_restore(cpu_t cpu_sr)
{
    __set_PRIMASK(cpu_sr);
}

/* 中断/临界区宏 -------------------------------------------------------------*/
/* 全局中断使能/禁止 */
#define CPU_EnableInterrupts()              cpu_irq_enable()
#define CPU_DisableInterrupts()             cpu_irq_disable()

/*
    CPU_EnterCritical()和CPU_ExitCritical(x),
    适用于线程函数的临界资源保护, 也可以用于中断函数的临界资源保护,
    在中断函数中推荐使用FromISR版本代替, 它针对中断进行了优化
*/
#define CPU_EnterCritical()                 cpu_irq_save()
#define CPU_ExitCritical(x)                 cpu_irq_restore(x)

/*
    CPU_EnterCriticalFromISR()和CPU_ExitCriticalFromISR(x),
    仅能用于中断函数的临界资源保护, 并且对非嵌套中断的情况进行了优化,
    禁止在线程函数中使用FromISR版本的函数
*/
#ifdef CPU_INTERRUPT_NOT_NESTING
    #define CPU_EnterCriticalFromISR()      ( 0 )
    #define CPU_ExitCriticalFromISR(x)      ( (void)(x) )
#else
    #define CPU_EnterCriticalFromISR()      cpu_irq_save()
    #define CPU_ExitCriticalFromISR(x)      cpu_irq_restore(x)
#endif

/* Cortex-M位带操作 ----------------------------------------------------------*/
#define BITBAND(addr, bitnum)   (((addr)&0xF0000000)+0x2000000+(((addr)&0xFFFFF)<<5)+((bitnum)<<2))
#define MEM_ADDR(addr)          *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum)  MEM_ADDR(BITBAND(addr, bitnum))

/* 底层操作宏 ----------------------------------------------------------------*/
#define CPU_NOP()               __NOP()

/* 调试相关宏 ----------------------------------------------------------------*/
/*调试断言*/
#if CPU_ASSERT_EN
    #define CPU_Assert(expr)    do { if (!(expr)) {CPU_DisableInterrupts();while(1);} } while(0)
#else
    #define CPU_Assert(expr)    ((void)0)
#endif

/*调试代码覆盖*/
#if CPU_COVERAGE_EN
    #define CPU_Coverage()      CPU_NOP()
#else
    #define CPU_Coverage()      ((void)0)
#endif

/*调试输出*/
#if CPU_PRINTF_EN
    #include <stdio.h>
    #define CPU_Printf(...)     printf(__VA_ARGS__)
#else
    #define CPU_Printf(...)     ((void)0)
#endif

/* 接口函数 ------------------------------------------------------------------*/
void cpu_PortInit(void);

#endif  /* __CPU_PORT_H */

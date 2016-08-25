/*******************************************************************************
* MCU型 号: STM32F1XX
* 文 件 名: cpu_config.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: CPU配置文件
*******************************************************************************/

#ifndef __CPU_CONFIG_H
#define __CPU_CONFIG_H

/* CPU参数配置 ---------------------------------------------------------------*/
#define CPU_FREQ_HZ         ( (uint32_t) 72000000 )  /* CPU运行频率(Hz)       */
#define CPU_TIMER_HZ        ( (uint32_t) 72000000 )  /* CPU节拍定时器频率(Hz) */
#define CPU_TICK_HZ         ( (uint32_t) 1000 )      /* CPU节拍频率(Hz)       */

/* CPU调试配置 ---------------------------------------------------------------*/
#define CPU_ASSERT_EN       ( 1 )                    /* 调试断言功能使能      */
#define CPU_COVERAGE_EN     ( 1 )                    /* 调试代码覆盖功能使能  */
#define CPU_PRINTF_EN       ( 1 )                    /* 调试输出功能使能      */

/* CPU宏定义 -----------------------------------------------------------------*/
#define CPU_TICK_PERIOD_IS_1MS
/* #define CPU_USE_16BIT_TICK */
/* #define CPU_INTERRUPT_NOT_NESTING */

/* OS宏定义 ------------------------------------------------------------------*/
#define CPU_USE_OS_SCHEDULER
/* #define CPU_USE_OS_FREERTOS */

#endif  /* __CPU_CONFIG_H */

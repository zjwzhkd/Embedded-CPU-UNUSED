/*******************************************************************************
* MCU型 号: STM8S
* 文 件 名: cpu_tick.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-10-13
* 文件说明: CPU节拍实现以及时间管理
*******************************************************************************/

#ifndef __CPU_TICK_H
#define __CPU_TICK_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpu_port.h"
#include "cpulib_list.h"

/* 数据结构 ------------------------------------------------------------------*/
/*节拍处理函数类型*/
typedef void (*TickIRQHandler_t) (void);
/*节拍中断请求结构体类型*/
typedef struct tick_irq TickIRQ_t;
struct tick_irq
{
    tick_t              period; /*产生节拍的周期  */
    tick_t volatile     count;  /*内部计数器      */
    TickIRQHandler_t    isr;    /*节拍中断服务函数*/
    ListNode_t          node;   /*节拍链表结点    */
};

/* 节拍转换宏 ----------------------------------------------------------------*/
#ifdef CPU_TICK_PERIOD_IS_1MS
    #define CPU_MS_TO_TICK(nms)     ( nms )
    #define CPU_TICK_TO_MS(ntick)   ( ntick )
#else
    #define CPU_MS_TO_TICK(nms)     ( (uint32_t)(nms)*CPU_TICK_HZ/1000 )
    #define CPU_TICK_TO_MS(ntick)   ( (uint32_t)(ntick)*1000/CPU_TICK_HZ )
#endif

/* 接口函数 ------------------------------------------------------------------*/
void cpu_TickInit(void);
void cpu_TickIRQRegister(TickIRQ_t *irq, tick_t period, TickIRQHandler_t isr);
void cpu_TickHandler(void);
void cpu_Delay(uint32_t n);
void cpu_DelayMs(uint16_t nms);

#endif  /* __CPU_TICK_H */

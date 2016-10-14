/*******************************************************************************
* MCU型 号: STM32F1XX
* 文 件 名: cpu_tick.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: CPU节拍实现以及时间管理
*******************************************************************************/

#include "cpu_tick.h"
/*******************************************************************************

                                    全局变量

*******************************************************************************/
static ListHead_t cpuTickIRQList;
static uint32_t fac_ms = 0;

/*******************************************************************************

                                    节拍函数

*******************************************************************************/
/*CPU节拍以及时间管理初始化*/
void cpu_TickInit(void)
{
#ifdef CPU_TICK_PERIOD_IS_1MS
    CPU_Assert(CPU_TICK_HZ == 1000);
#endif
    list_Init(&cpuTickIRQList);
    fac_ms = CPU_TIMER_HZ/1000;
    /*
     * 初始化Timer4
     * CPU_TIMER_HZ = CPU_FREQ_HZ/64
     * PERIOD = CPU_TIMER_HZ/CPU_TICK_HZ-1
     * 开启UPDATE中断
     */
    TIM4_TimeBaseInit(TIM4_PRESCALER_64, CPU_TIMER_HZ/CPU_TICK_HZ-1);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
}

/**
 * 注册节拍中断请求
 *
 * @param irq: 待注册的节拍中断请求的结构体指针
 *
 * @param period: 节拍中断请求的周期
 *
 * @param isr: 节拍中断请求的处理函数指针
 */
void cpu_TickIRQRegister(TickIRQ_t *irq, tick_t period, TickIRQHandler_t isr)
{
cpu_t cpu_sr;

    /*参数检验*/
    CPU_Assert(0 != period);
    CPU_Assert(0 != isr);
    /*注册节拍中断*/
    list_Init(&irq->node);
    irq->period = period;
    irq->count  = period-1;
    irq->isr    = isr;
    cpu_sr = CPU_EnterCritical();
    {
        list_Add(&cpuTickIRQList, &irq->node);
    }
    CPU_ExitCritical(cpu_sr);
}

/**
 * (总)节拍中断处理函数
 *
 * @note: 以CPU_TICK_HZ的频率, 在CPU节拍中断函数中调用
 */
void cpu_TickHandler(void)
{
ListNode_t *pos;
TickIRQ_t  *irq;

    list_for_each(pos, &cpuTickIRQList)
    {
        irq = list_entry(pos, TickIRQ_t, node);
        if (irq->period > 0)
        {
            if (irq->count > 0)
            {
                irq->count--;
            }
            else
            {
                irq->count = irq->period - 1;
                (irq->isr)();
            }
        }
    }
}

/*******************************************************************************

                                    时间管理

*******************************************************************************/
/**
 * 定时器节拍延时函数
 *
 * @param n: 延时时间(1/CPU_TIMER_HZ)
 */
void cpu_Delay(uint32_t n)
{
uint32_t totalCnt = n, tcnt = 0;
uint8_t tnew, told, reload = TIM4->ARR + 1;

    told = TIM4->CNTR;
    for ( ;; )
    {
        tnew = TIM4->CNTR;
        if (tnew != told)
        {
            if (tnew>told) tcnt += tnew - told;
            else tcnt += reload + tnew - told;
            told = tnew;
            if (tcnt>=totalCnt) break;
        }
    }
}

/**
 * 毫秒级延时函数
 *
 * @param nms: 延时时间(ms)
 */
void cpu_DelayMs(uint16_t nms)
{
uint32_t tmp_n;

    tmp_n = (uint32_t)fac_ms*nms;
    cpu_Delay(tmp_n);
}

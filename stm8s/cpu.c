/*******************************************************************************
* MCU型 号: STM8S
* 文 件 名: cpu.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-10-13
* 文件说明: CPU底层实现
*******************************************************************************/

#include "cpu.h"

static void prvSystemClockConfig(void);
/*******************************************************************************

                                    接口函数

*******************************************************************************/
/*CPU初始化*/
void cpu_Init(void)
{
    /*中断控制器设置*/
    CPU_DisableInterrupts();
    ITC_DeInit();
    /*系统时钟配置*/
    prvSystemClockConfig();
    /*初始化CPU节拍*/
    cpu_TickInit();
}

/*******************************************************************************

                                    私有函数

*******************************************************************************/
/*系统时钟配置*/
static void prvSystemClockConfig(void)
{
uint16_t timeout = CLK_TIMEOUT;

    CLK->SWCR |=  CLK_SWCR_SWEN;    /* 使能时钟自动切换 */
    CLK->SWCR &= ~CLK_SWCR_SWIEN;   /* 禁用时钟切换中断 */
    CLK->SWR   =  CLK_SOURCE_HSE;   /* 时钟切换至HSE    */

    /* 等待时钟切换完成 */
    while (((CLK->SWCR&CLK_SWCR_SWBSY) != 0) && (timeout != 0))
    {
        timeout--;
    }

    if (0 != timeout)
    {
        /* 切换成功, 关闭内部时钟HSI */
        CLK->ICKR &= ~CLK_ICKR_HSIEN;
    }
    else
    {
        /* 切换失败, 设置HSI预分频, f_HSI = f_HSE */
        CLK->CKDIVR &= ~CLK_CKDIVR_HSIDIV;
        CLK->CKDIVR |=  CLK_PRESCALER_HSIDIV2;
    }

    CPU_Assert(CPU_FREQ_HZ == CLK_GetClockFreq());
}

/*******************************************************************************
* MCU型 号: STM32F1XX
* 文 件 名: cpu_port.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: CPU接口函数实现
*******************************************************************************/

#include "cpu_port.h"

static void prvSystemClockConfig(void);
/*******************************************************************************

                                    中断管理

*******************************************************************************/
/**
 * 设置优先级分组
 *
 * @param PriorityGroup: 允许数值0..7, 表示8位优先级数值, 子优先级位域最高位, STM32仅支持3..7
 *        ======================================================================
 *        PriorityGroup | PreemptPriority | SubPriority |   Description
 *        ======================================================================
 *              7       |       0         |     0-15    |   0位抢占优先级
 *                      |                 |             |   4位子优先级
 *        ----------------------------------------------------------------------
 *              6       |       0-1       |     0-7     |   1位抢占优先级
 *                      |                 |             |   3位子优先级
 *        ----------------------------------------------------------------------
 *              5       |       0-3       |     0-3     |   2位抢占优先级
 *                      |                 |             |   2位子优先级
 *        ----------------------------------------------------------------------
 *              4       |       0-7       |     0-1     |   3位抢占优先级
 *                      |                 |             |   1位子优先级
 *        ----------------------------------------------------------------------
 *              3       |       0-15      |     0       |   4位抢占优先级
 *                      |                 |             |   0位子优先级
 *        ----------------------------------------------------------------------
 *
 */
void cpu_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    NVIC_SetPriorityGrouping(PriorityGroup);
}

/**
 * 设置中断优先级
 *
 * @param IRQn: 中断号
 *
 * @param PreemptPriority: 中断抢占优先级, 0..15
 *
 * @param SubPriority: 中断子优先级, 0..15
 */
void cpu_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
uint32_t prioritygroup = 0x00;

    prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

/**
 * 使能指定外设中断
 *
 * @param IRQn: 外设中断号, 非负值
 */
void cpu_NVIC_EnableIRQ(IRQn_Type IRQn)
{
    NVIC_EnableIRQ(IRQn);
}

/**
 * 禁止指定外设中断
 *
 * @param IRQn: 外设中断号, 非负值
 */
void cpu_NVIC_DisableIRQ(IRQn_Type IRQn)
{
    NVIC_DisableIRQ(IRQn);
}

/*******************************************************************************

                                    接口管理

*******************************************************************************/
/*CPU接口初始化函数*/
void cpu_PortInit(void)
{
    /*系统时钟配置*/
    prvSystemClockConfig();
    /*使能堆栈8字节对齐*/
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
    /*配置优先级分组, 4位抢占优先级*/
    cpu_NVIC_SetPriorityGrouping(3);
}

/*******************************************************************************

                                    私有函数

*******************************************************************************/
/*系统时钟配置*/
static void prvSystemClockConfig(void)
{
    /*
        在SystemInit()函数中, 实现了:
        1.复位时钟配置
        2.可选的外部SRAM初始化
        3.内部Flash预取缓存和等待状态设置
        4.CPU系统的时钟初始化
        5.中断向量表偏移设置
    */
    SystemCoreClockUpdate();
    CPU_Assert(CPU_FREQ_HZ == SystemCoreClock);
}

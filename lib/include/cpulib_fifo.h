/*******************************************************************************
* 文 件 名: cpulib_fifo.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-09-02
* 文件说明: 队列实现
*******************************************************************************/

#ifndef __CPULIB_FIFO_H
#define __CPULIB_FIFO_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpulib_def.h"

/* 数据结构 ------------------------------------------------------------------*/
typedef void FIFO_t;
struct __fifo
{
    size_t      in;     /* 写索引   */
    size_t      out;    /* 读索引   */
    size_t      total;  /* 总元素数 */
    size_t      count;  /* 已使用量 */
    size_t      esize;  /* 元素大小 */
    void       *data;   /* 存储地址 */
};

#define __STRUCT_FIFO_COMMON(datatype)  \
    union {                             \
        struct __fifo       fifo;       \
        datatype           *type;       \
    }

/*
 * FIFO结构体类型
 * type: FIFO元素类型
 * size: FIFO元素个数
 */
#define STRUCT_FIFO(type, size)         \
    struct {                            \
        __STRUCT_FIFO_COMMON(type);     \
        type        buf[size];          \
    }

/*
 * 初始化FIFO
 * pfifo: FIFO结构体指针, STRUCT_FIFO(type, size)的指针类型
 */
#define INIT_FIFO(pfifo)    do                              \
{                                                           \
    struct __fifo *tmp_pfifo    = &((pfifo)->fifo);         \
    const size_t tmp_end        = ARRAY_SIZE((pfifo)->buf); \
    const size_t tmp_esize      = sizeof(*((pfifo)->type)); \
    void * const tmp_data       = (pfifo)->buf;             \
    tmp_pfifo->in               = 0;                        \
    tmp_pfifo->out              = 0;                        \
    tmp_pfifo->total            = tmp_end;                  \
    tmp_pfifo->count            = 0;                        \
    tmp_pfifo->esize            = tmp_esize;                \
    tmp_pfifo->data             = tmp_data;                 \
} while (0)

/* 操作函数 ------------------------------------------------------------------*/
void fifo_Reset( FIFO_t *pfifo );
size_t fifo_In( FIFO_t *pfifo, const void *buffer, size_t len );
size_t fifo_Out( FIFO_t *pfifo, void *buffer, size_t len );
size_t fifo_Peek( FIFO_t *pfifo, void *buffer, size_t len );

bool fifo_IsEmpty( FIFO_t *pfifo );
bool fifo_IsFull( FIFO_t *pfifo );
size_t fifo_GetCount( FIFO_t *pfifo );
size_t fifo_GetTotal( FIFO_t *pfifo );
size_t fifo_GetAvail( FIFO_t *pfifo );

#endif  /* __CPULIB_FIFO_H */

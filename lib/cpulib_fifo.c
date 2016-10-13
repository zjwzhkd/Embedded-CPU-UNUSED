/*******************************************************************************
* 文 件 名: cpulib_fifo.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-09-02
* 文件说明: 队列实现
*******************************************************************************/

#include "cpulib_fifo.h"
#include <string.h>
/*******************************************************************************

                                    私有函数

*******************************************************************************/
/**
 * FIFO进队列内容复制
 *
 * @param pFIFO: FIFO指针
 *
 * @param src: 进队列源内存地址
 *
 * @param len: 进队列元素长度
 */
static void prvFifoCopyIn(struct __fifo *pFIFO, const void *src, size_t len)
{
uint8_t *dest = (uint8_t *)(pFIFO->data);
size_t esize  = pFIFO->esize;
size_t total  = pFIFO->total;
size_t off    = pFIFO->in;
size_t l;

    if ( 1 != esize )
    {
        off   *= esize;
        total *= esize;
        len   *= esize;
    }
    l = ( (total - off) < len ) ? (total - off) : len;
    memcpy(dest + off, src, l);
    memcpy(dest, (uint8_t *)src + l, len - l);
}

/**
 * FIFO出队列内容复制
 *
 * @param pFIFO: FIFO指针
 *
 * @param dest: 出队列目标内存地址
 *
 * @param len: 出队列元素长度
 */
static void prvFifoCopyOut(struct __fifo *pFIFO, void *dest, size_t len)
{
uint8_t const *src = (uint8_t *)(pFIFO->data);
size_t esize = pFIFO->esize;
size_t total = pFIFO->total;
size_t off   = pFIFO->out;
size_t l;

    if ( 1 != esize )
    {
        off   *= esize;
        total *= esize;
        len   *= esize;
    }
    l = ( (total - off) < len ) ? (total - off) : len;
    memcpy(dest, src + off, l);
    memcpy((uint8_t *)dest + l, src, len - l);
}

/*******************************************************************************

                                    操作函数

*******************************************************************************/
/**
 * 重置FIFO
 *
 * @param pfifo: 待重置的FIFO指针
 */
void fifo_Reset( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    pFIFO->in    = 0;
    pFIFO->out   = 0;
    pFIFO->count = 0;
}

/**
 * FIFO进队列
 *
 * @param pfifo: FIFO指针
 *
 * @param buffer: 保存进队列元素的缓存地址
 *
 * @len: 进队列的元素个数, 不一定能够全部成功进队列
 *
 * @return: 返回成功进队列的元素个数, 不超过len
 */
size_t fifo_In( FIFO_t *pfifo, const void *buffer, size_t len )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    if ( len > (pFIFO->total - pFIFO->count) )
    {
        len = pFIFO->total - pFIFO->count;
    }
    prvFifoCopyIn(pFIFO,buffer,len);
    pFIFO->count += len;
    pFIFO->in += len;
    if ( (pFIFO->in > pFIFO->total) || (pFIFO->in < len) )
    {
        pFIFO->in -= pFIFO->total;
        debug_assert(pFIFO->in < pFIFO->total);
    }
    return (len);
}

/**
 * FIFO出队列
 *
 * @param pfifo: FIFO指针
 *
 * @param buffer: 保存出队列元素的缓存地址
 *
 * @len: 出队列的元素个数, 不一定能够全部成功出队列
 *
 * @return: 返回成功出队列的元素个数, 不超过len
 */
size_t fifo_Out( FIFO_t *pfifo, void *buffer, size_t len )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    if ( len > pFIFO->count )
    {
        len = pFIFO->count;
    }
    prvFifoCopyOut(pFIFO,buffer,len);
    pFIFO->count -= len;
    pFIFO->out += len;
    if ( (pFIFO->out > pFIFO->total) || (pFIFO->out < len) )
    {
        pFIFO->out -= pFIFO->total;
        debug_assert(pFIFO->out < pFIFO->total);
    }
    return (len);
}

/**
 * 读取FIFO出队列元素, 但不进行出队列操作
 *
 * @param pfifo: FIFO指针
 *
 * @param buffer: 保存读取结果的缓存地址
 *
 * @len: 要读取的出队列元素个数, 不一定能够全部成功读取
 *
 * @return: 返回成功读取的元素个数, 不超过len
 */
size_t fifo_Peek( FIFO_t *pfifo, void *buffer, size_t len )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    if ( len > pFIFO->count )
    {
        len = pFIFO->count;
    }
    prvFifoCopyOut(pFIFO,buffer,len);
    return (len);
}

/**
 * 判断FIFO是否为空
 *
 * @param pfifo: FIFO指针
 *
 * @return: 返回布尔值, true表示FIFO为空
 */
bool fifo_IsEmpty( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    if ( 0 == pFIFO->count )
    {
        debug_assert(pFIFO->in == pFIFO->out);
        return (true);
    }
    else
    {
        debug_assert(pFIFO->in != pFIFO->out);
        return (false);
    }
}

/**
 * 判断FIFO是否已满
 *
 * @param pfifo: FIFO指针
 *
 * @return: 返回布尔值, true表示FIFO已满
 */
bool fifo_IsFull( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    if ( pFIFO->count == pFIFO->total )
    {
        debug_assert(pFIFO->in == pFIFO->out);
        return (true);
    }
    else
    {
        debug_assert(pFIFO->in != pFIFO->out);
        return (false);
    }
}

/**
 * 获取FIFO已经保存的元素个数
 *
 * @param pfifo: FIFO指针
 *
 * @return: 返回FIFO已经保存的元素个数
 */
size_t fifo_GetCount( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    return (pFIFO->count);
}

/**
 * 获取FIFO总共可以保存的元素个数
 *
 * @param pfifo: FIFO指针
 *
 * @return: 返回FIFO总共可以保存的元素个数
 */
size_t fifo_GetTotal( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    return (pFIFO->total);
}

/**
 * 获取FIFO剩余可以保存的元素个数
 *
 * @param pfifo: FIFO指针
 *
 * @return: 返回FIFO剩余可以保存的元素个数
 */
size_t fifo_GetAvail( FIFO_t *pfifo )
{
struct __fifo *pFIFO = (struct __fifo *)pfifo;

    return (pFIFO->total - pFIFO->count);
}

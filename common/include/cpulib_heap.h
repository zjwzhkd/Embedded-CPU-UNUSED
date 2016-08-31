/*******************************************************************************
* 文 件 名: cpulib_heap.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-27
* 文件说明: 动态内存分配
*******************************************************************************/

#ifndef __CPULIB_HEAP_H
#define __CPULIB_HEAP_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpulib_def.h"

/* 字节对齐 ------------------------------------------------------------------*/
#define HEAP_BYTE_ALIGNMENT         ( CPU_BYTE_ALIGNMENT )
#define HEAP_BYTE_ALIGNMENT_MASK    ( HEAP_BYTE_ALIGNMENT-1 )

/* 数据类型 ------------------------------------------------------------------*/
/*Heap设备类型*/
typedef void HeapDev_t;
/*Heap信息类型*/
typedef struct heap_info HeapInfo_t;
struct heap_info
{
    size_t  totalSize;              /*Heap内存总大小      */
    size_t  freeSize;               /*Heap未分配内存大小  */
    size_t  minimumEverFreeSize;    /*Heap内存最小剩余量  */
    size_t  freeBlocks;             /*Heap不连续空闲块数量*/
};

/* 操作函数 ------------------------------------------------------------------*/
/*Heap设备操作函数*/
HeapDev_t *heap_Create( uint8_t *startAddr, size_t totalSize );
void heap_GetInfo( HeapDev_t *heap, HeapInfo_t *info );
/*动态内存分配函数*/
void *heap_Malloc( HeapDev_t *heap, size_t size );
void *heap_Calloc( HeapDev_t *heap, size_t nmemb, size_t size );
void *heap_Realloc( HeapDev_t *heap, void *ptr, size_t size );
void heap_Free( HeapDev_t *heap, void *ptr );
/*内存复制函数*/
void *heap_Memcpy( void *dest, const void *src, size_t n );
void *heap_Memset( void *dest, int c, size_t n );

#endif  /* __CPULIB_HEAP_H */

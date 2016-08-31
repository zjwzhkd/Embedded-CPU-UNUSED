/*******************************************************************************
* 文 件 名: cpulib_heap.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-27
* 文件说明: 动态内存分配
*******************************************************************************/

#include "cpulib_heap.h"
/*******************************************************************************

                                    数据结构

*******************************************************************************/
/*Heap内存块*/
typedef struct heap_block HeapBlock_t;
struct heap_block
{
    HeapBlock_t    *pNextFreeBlock;
    size_t          blockSize;
};
/*Heap首部记录*/
typedef struct heap_head HeapHead_t;
struct heap_head
{
    HeapBlock_t     startBlock;             /*Heap空闲内存块链表头*/
    HeapBlock_t    *pEndBlock;              /*Heap空闲内存块尾结点*/
    size_t          totalSize;              /*Heap内存总大小      */
    size_t          freeSize;               /*Heap未分配内存大小  */
    size_t          minimumEverFreeSize;    /*Heap内存最小剩余量  */
};

/*******************************************************************************

                                     宏定义

*******************************************************************************/
/*
 * 判断内存大小是否满足字节对齐
 * size:   内存大小(size_t类型)
 * return: 布尔值, 若对齐返回真
 */
#define __HEAP_IS_SIZE_ALIGNED(size)    ( 0 == ( ((size_t)(size)) & ((size_t)HEAP_BYTE_ALIGNMENT_MASK) ) )

/*
 * 对指定的内存大小, 进行字节对齐处理
 * size:   给定的内存大小
 * return: 字节对齐后的大小
 */
#define __HEAP_GET_SIZE_ALIGNED(size)   \
    ( ((size_t)(size) + (size_t)HEAP_BYTE_ALIGNMENT_MASK) & (~((size_t)HEAP_BYTE_ALIGNMENT_MASK)) )

/*
 * 判断指针是否满足字节对齐
 * ptr:    给定的指针
 * return: 布尔值, 若对齐返回真
 */
#define __HEAP_IS_PTR_ALIGNED(ptr)      __HEAP_IS_SIZE_ALIGNED(ptr)

/*
 * 对指定的指针, 进行字节对齐处理, 并进行类型转换
 * pAlign: 保存字节对齐结果的指针
 * type:   字节对齐指针的类型
 * pSrc:   待转换的指针
 */
#define __HEAP_SET_PTR_ALIGNED(pAlign,type,pSrc)    do                      \
{                                                                           \
const uint8_t *_pcu8 = (uint8_t *)(pSrc);                                   \
const size_t _cs = (size_t)(pSrc);                                          \
    pAlign = (type) (   (__HEAP_IS_PTR_ALIGNED(_pcu8)) ? (_pcu8) :          \
                        ( _pcu8 +                                           \
                            ( (size_t)HEAP_BYTE_ALIGNMENT -                 \
                                ( _cs&((size_t)HEAP_BYTE_ALIGNMENT_MASK) )  \
                            )                                               \
                        )                                                   \
                    );                                                      \
} while(0)

/*******************************************************************************

                                    常量定义

*******************************************************************************/
/*Heap首部结构体大小*/
static const size_t HeapHeadStructSize    = __HEAP_GET_SIZE_ALIGNED(sizeof(HeapHead_t));
/*Heap内存块结构体大小*/
static const size_t HeapBlockStructSize   = __HEAP_GET_SIZE_ALIGNED(sizeof(HeapBlock_t));
/*Heap最小可分割内存块大小*/
static const size_t HeapMinimumBlockSize  = 2 * __HEAP_GET_SIZE_ALIGNED(sizeof(HeapBlock_t));
/*Heap内存块分配标志位*/
static const size_t HeapBlockAllocatedBit = ((size_t)1) << (8*sizeof(size_t)-1);

static void prvInsertBlockIntoFreeList(HeapHead_t *pHeap, HeapBlock_t *pBlockToInsert);
/*******************************************************************************

                                  Heap操作函数

*******************************************************************************/
/**
 * 创建Heap设备
 *
 * @param startAddr: Heap内存起始地址
 *
 * @param totalSize: Heap内存总大小
 *
 * @return: 若创建成功, 返回Heap设备指针
 *          若创建失败, 返回NULL
 */
HeapDev_t *heap_Create( uint8_t *startAddr, size_t totalSize )
{
HeapHead_t  *pHeap = NULL;
HeapBlock_t *pBlock;
uint8_t     *pAligned;
size_t       freeSize;

    __HEAP_SET_PTR_ALIGNED(pAligned, uint8_t *, startAddr);
    freeSize = totalSize - ((size_t)pAligned - (size_t)startAddr);
    freeSize &= ~( (size_t)HEAP_BYTE_ALIGNMENT_MASK );

    if  (   ( freeSize < (HeapHeadStructSize+HeapMinimumBlockSize+HeapBlockStructSize) ) ||
            ( freeSize & HeapBlockAllocatedBit)
        )
    {
        pHeap = NULL;
    }
    else
    {
        /*分配Heap首部*/
        pHeap = (HeapHead_t *)pAligned;
        pAligned += HeapHeadStructSize;
        freeSize -= HeapHeadStructSize;
        /*分配内存块尾结点*/
        freeSize -= HeapBlockStructSize;
        pHeap->pEndBlock = (HeapBlock_t *)(pAligned + freeSize);
        pHeap->pEndBlock->pNextFreeBlock = NULL;
        pHeap->pEndBlock->blockSize = 0;
        /*分配空闲内存块*/
        pBlock = (HeapBlock_t *)pAligned;
        pBlock->pNextFreeBlock = pHeap->pEndBlock;
        pBlock->blockSize = freeSize;
        /*初始化Heap首部*/
        pHeap->startBlock.pNextFreeBlock = pBlock;
        pHeap->startBlock.blockSize = 0;
        pHeap->totalSize = totalSize;
        pHeap->freeSize  = freeSize;
        pHeap->minimumEverFreeSize = freeSize;
    }
    return (pHeap);
}

/**
 * 获取Heap设备信息
 *
 * @param heap: Heap设备指针
 *
 * @param info: 保存设备信息的结构体指针
 */
void heap_GetInfo( HeapDev_t *heap, HeapInfo_t *info )
{
HeapHead_t *pHeap;
HeapBlock_t *pBlock;

    /*参数检验*/
    debug_assert(NULL != heap);
    debug_assert(NULL != info);
    pHeap = (HeapHead_t *)heap;
    info->totalSize = pHeap->totalSize;
    info->freeSize  = pHeap->freeSize;
    info->minimumEverFreeSize = pHeap->minimumEverFreeSize;
    for (   info->freeBlocks = 0, pBlock = pHeap->startBlock.pNextFreeBlock;
            pBlock != pHeap->pEndBlock;
            ++(info->freeBlocks), pBlock = pBlock->pNextFreeBlock
        )
    {
    }
}

/*******************************************************************************

                                动态内存分配函数

*******************************************************************************/
/**
 * 内存分配函数(malloc)实现
 *
 * @param heap: Heap设备指针
 *
 * @param size: 分配内存的大小
 *
 * @return: 若分配成功, 返回分配的内存起始地址,
 *          若分配失败, 返回NULL
 */
void *heap_Malloc( HeapDev_t *heap, size_t size )
{
void *pRet = NULL;
HeapHead_t *pHeap;
HeapBlock_t *pPrevBlock, *pNewBlock, *pBlock;

    /*参数检验*/
    debug_assert(NULL != heap);
    pHeap = (HeapHead_t *)heap;
    if ( (size == 0) || (size&HeapBlockAllocatedBit) )
    {
        pRet = NULL;
    }
    else
    {
        size += HeapBlockStructSize;
        size  = __HEAP_GET_SIZE_ALIGNED(size);
        if (size > pHeap->freeSize)
        {
            pRet = NULL;
        }
        else
        {
            for (   pPrevBlock = &pHeap->startBlock, pBlock = pHeap->startBlock.pNextFreeBlock;
                    (pBlock->blockSize < size) && (pBlock->pNextFreeBlock != NULL);
                    pPrevBlock = pBlock, pBlock = pBlock->pNextFreeBlock
                )
            {
            }
            if ( pBlock != pHeap->pEndBlock )
            {
                pRet = (void *)( ((uint8_t *)pBlock) + HeapBlockStructSize );
                pPrevBlock->pNextFreeBlock = pBlock->pNextFreeBlock;
                /*分割Block*/
                if ( (pBlock->blockSize - size) > HeapMinimumBlockSize )
                {
                    pNewBlock = (HeapBlock_t *)( ((uint8_t *)pBlock) + size );
                    debug_assert(__HEAP_IS_PTR_ALIGNED(pNewBlock));
                    pNewBlock->blockSize = pBlock->blockSize - size;
                    pNewBlock->pNextFreeBlock = NULL;
                    pBlock->blockSize = size;
                    prvInsertBlockIntoFreeList(pHeap, pNewBlock);
                }
                /*更新Heap首部*/
                pHeap->freeSize -= pBlock->blockSize;
                if ( pHeap->minimumEverFreeSize > pHeap->freeSize )
                {
                    pHeap->minimumEverFreeSize = pHeap->freeSize;
                }
                /*标记已分配内存块*/
                pBlock->pNextFreeBlock = NULL;
                pBlock->blockSize |= HeapBlockAllocatedBit;
            }
        }
    }
    debug_assert(__HEAP_IS_PTR_ALIGNED(pRet));
    return (pRet);
}

/**
 * 内存分配函数(calloc)实现
 *
 * @param heap: Heap设备指针
 *
 * @param nmemb: 分配元素的个数
 *
 * @param size: 单个元素的大小
 *
 * @return: 若分配成功, 返回分配的内存起始地址,
 *          若分配失败, 返回NULL
 */
void *heap_Calloc( HeapDev_t *heap, size_t nmemb, size_t size )
{
void *pRet = NULL;
size_t totalSize = nmemb*size;

    pRet = heap_Malloc(heap, totalSize);
    if ( NULL != pRet )
    {
        heap_Memset(pRet, 0, size);
    }
    return (pRet);
}

/**
 * 内存分配函数(realloc)实现
 *
 * @param heap: Heap设备指针
 *
 * @param ptr: 原先已分配的内存起始地址, 若为NULL则等同于malloc
 *
 * @param size: 重新分配内存的大小, 若为0则等同于free
 *
 * @return: 若分配成功, 返回重新分配的内存起始地址,
 *          若分配失败, 返回NULL
 */
void *heap_Realloc( HeapDev_t *heap, void *ptr, size_t size )
{
void *pRet = NULL;
HeapHead_t *pHeap;
HeapBlock_t *pBlock, *pNewBlock;
size_t wantedSize;
size_t allocSize;

    /*参数检验*/
    debug_assert(NULL != heap);
    pHeap = (HeapHead_t *)heap;

    if (NULL == ptr)
    {
        pRet = heap_Malloc(heap, size);
    }
    else if (0 == size)
    {
        heap_Free(heap, ptr);
        pRet = NULL;
    }
    else if (size&HeapBlockAllocatedBit)
    {
        pRet = NULL;
    }
    else
    {
        /*计算重新分配的内存大小*/
        wantedSize  = size;
        wantedSize += HeapBlockStructSize;
        wantedSize  = __HEAP_GET_SIZE_ALIGNED(wantedSize);
        /*获取原先已分配的内存大小*/
        debug_assert(__HEAP_IS_PTR_ALIGNED(ptr));
        debug_assert((uint8_t *)ptr > (uint8_t *)pHeap);
        debug_assert((uint8_t *)ptr < (uint8_t *)(pHeap->pEndBlock));
        pBlock = (HeapBlock_t *)( ((uint8_t *)ptr) - HeapBlockStructSize );
        debug_assert(pBlock->blockSize & HeapBlockAllocatedBit);
        debug_assert(pBlock->pNextFreeBlock == NULL);
        allocSize   = pBlock->blockSize & (~HeapBlockAllocatedBit);
        debug_assert(allocSize > HeapBlockStructSize);

        if (wantedSize > allocSize)
        {
            /*重新分配内存*/
            pRet = heap_Malloc(heap, size);
            if (pRet != NULL)
            {
                heap_Memcpy(pRet,ptr,allocSize-HeapBlockStructSize);
                heap_Free(heap, ptr);
            }
        }
        else
        {
            /*分割原有内存*/
            pRet = ptr;
            if ( (allocSize-wantedSize) > HeapMinimumBlockSize )
            {
                pNewBlock = (HeapBlock_t *)( ((uint8_t *)pBlock) + wantedSize );
                debug_assert(__HEAP_IS_PTR_ALIGNED(pNewBlock));
                pNewBlock->blockSize = allocSize - wantedSize;
                pNewBlock->pNextFreeBlock = NULL;
                pBlock->blockSize = wantedSize | HeapBlockAllocatedBit;
                pBlock->pNextFreeBlock = NULL;
                prvInsertBlockIntoFreeList(pHeap, pNewBlock);
            }
        }
    }
    return (pRet);
}

/**
 * 内存释放函数(free)实现
 *
 * @param heap: Heap设备指针
 *
 * @param size: 待释放的内存起始地址, 若为NULL则无动作
 */
void heap_Free( HeapDev_t *heap, void *ptr )
{
HeapHead_t *pHeap;
HeapBlock_t *pBlock;

    if (NULL != ptr)
    {
        /*参数检验*/
        debug_assert(NULL != heap);
        pHeap = (HeapHead_t *)heap;
        debug_assert((uint8_t *)ptr > (uint8_t *)pHeap);
        debug_assert((uint8_t *)ptr < (uint8_t *)(pHeap->pEndBlock));
        pBlock = (HeapBlock_t *)( ((uint8_t *)ptr) - HeapBlockStructSize );
        debug_assert(pBlock->blockSize & HeapBlockAllocatedBit);
        debug_assert(pBlock->pNextFreeBlock == NULL);
        if ( pBlock->blockSize & HeapBlockAllocatedBit )
        {
            if ( pBlock->pNextFreeBlock == NULL )
            {
                pBlock->blockSize &= ~HeapBlockAllocatedBit;
                pHeap->freeSize += pBlock->blockSize;
                prvInsertBlockIntoFreeList(pHeap, pBlock);
            }
        }
    }
}

/*******************************************************************************

                                  内存复制函数

*******************************************************************************/
/*内存复制函数*/
void *heap_Memcpy( void *dest, const void *src, size_t n )
{
uint8_t *d = (uint8_t *)dest;
const uint8_t *s = (const uint8_t *)src;

    for (; 0 < n; ++d, ++s, --n)
        *d = *s;
    return (dest);
}

/*内存置位函数*/
void *heap_Memset( void *dest, int c, size_t n )
{
uint8_t *d = (uint8_t *)dest;
const uint8_t uc = (uint8_t)c;

    for (; 0 < n; ++d, --n)
        *d = uc;
    return (dest);
}

/*******************************************************************************

                                    私有函数

*******************************************************************************/
/*将孤立的内存块重新插入到空闲内存块链表, 并完成相邻内存块合并*/
static void prvInsertBlockIntoFreeList(HeapHead_t *pHeap, HeapBlock_t *pBlockToInsert)
{
HeapBlock_t *pSearch;
uint8_t *puc;

    /*
        搜索pBlockToInsert插入位置,
        pSearch .. pBlockToInsert .. pSearch->pNextFreeBlock
    */
    for (pSearch = &pHeap->startBlock; pSearch->pNextFreeBlock < pBlockToInsert; pSearch = pSearch->pNextFreeBlock)
    {
    }

    /*合并pSearch .. pBlockToInsert*/
    puc = (uint8_t *)pSearch;
    if ( puc + pSearch->blockSize == (uint8_t *)pBlockToInsert )
    {
        pSearch->blockSize += pBlockToInsert->blockSize;
        pBlockToInsert = pSearch;
    }

    /*合并pBlockToInsert .. pSearch->pNextFreeBlock*/
    puc = (uint8_t *)pBlockToInsert;
    if ( puc + pBlockToInsert->blockSize == (uint8_t *)pSearch->pNextFreeBlock )
    {
        if ( pSearch->pNextFreeBlock != pHeap->pEndBlock )
        {
            pBlockToInsert->blockSize += pSearch->pNextFreeBlock->blockSize;
            pBlockToInsert->pNextFreeBlock = pSearch->pNextFreeBlock->pNextFreeBlock;
        }
        else
        {
            pBlockToInsert->pNextFreeBlock = pHeap->pEndBlock;
        }
    }
    else
    {
        pBlockToInsert->pNextFreeBlock = pSearch->pNextFreeBlock;
    }

    /*修改pSearch->pNextFreeBlock*/
    if ( pBlockToInsert != pSearch )
    {
        pSearch->pNextFreeBlock = pBlockToInsert;
    }
}

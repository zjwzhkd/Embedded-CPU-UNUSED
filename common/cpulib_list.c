/*******************************************************************************
* 文 件 名: cpulib_list.c
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: 双向链表, 移植自Linux内核链表
*******************************************************************************/

#include "cpulib_list.h"
/*******************************************************************************

                                    操作函数

*******************************************************************************/
/**
 * 初始化链表头或者链表结点
 *
 * @param list: 待初始化的链表头或者链表结点指针
 */
void list_Init( struct list_head *list )
{
    list->next = list;
    list->prev = list;
}

/**
 * 判断链表是否为空或者判断链表结点是否孤立
 *
 * @param list: 链表头或者链表结点指针
 *
 * @return: 布尔值, 若为空返回true, 反之返回false
 */
bool list_IsEmpty( struct list_head *list )
{
    if (list->next == list)
    {
        debug_assert(list->prev == list);
        return (true);
    }
    else
    {
        debug_assert(list->prev != list);
        return (false);
    }
}

/**
 * 向链表起始位置添加一个链表结点
 *
 * @param head: 链表头指针
 *
 * @param node: 待添加的链表结点指针, 链表结点必须是孤立的
 */
void list_Add( ListHead_t *head, ListNode_t *node )
{
    debug_assert(list_IsEmpty(node));
    node->next       = head->next;
    node->prev       = head;
    node->next->prev = node;
    head->next       = node;
}

/**
 * 向链表尾部位置添加一个链表结点
 *
 * @param head: 链表头指针
 *
 * @param node: 待添加的链表结点指针, 链表结点必须是孤立的
 */
void list_AddTail( ListHead_t *head, ListNode_t *node )
{
    debug_assert(list_IsEmpty(node));
    node->next       = head;
    node->prev       = head->prev;
    node->prev->next = node;
    head->prev       = node;
}

/**
 * 从链表中移除指定的链表结点
 *
 * @param node: 待删除的链表结点指针
 */
void list_Del( ListNode_t *node )
{
    debug_assert(NULL != node->prev);
    debug_assert(NULL != node->next);
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next       = node;
    node->prev       = node;
}

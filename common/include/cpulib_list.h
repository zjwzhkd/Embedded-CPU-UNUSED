/*******************************************************************************
* 文 件 名: cpulib_list.h
* 创 建 者: Keda Huang
* 版    本: V1.0
* 创建日期: 2016-08-23
* 文件说明: 双向链表, 移植自Linux内核链表
*******************************************************************************/

#ifndef __CPULIB_LIST_H
#define __CPULIB_LIST_H

/* 头文件 --------------------------------------------------------------------*/
#include "cpulib_def.h"

/* 链表数据结构 --------------------------------------------------------------*/
struct list_head
{
    struct list_head *next;
    struct list_head *prev;
};
typedef struct list_head ListHead_t;  /*链表头结构类型  */
typedef struct list_head ListNode_t;  /*链表结点结构类型*/

/* 链表处理宏 ----------------------------------------------------------------*/
/*
 * 获取包含链表结点的结构体指针
 * ptr:    链表结点指针
 * type:   包含链表结点的结构体类型
 * member: 链表结点在结构体中的成员变量名
 * return: 结构体指针
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/*
 * 链表遍历
 * pos:  链表结点遍历指针
 * head: 链表头指针
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next;pos != (head) ;pos = pos->next)

/* 链表操作函数 --------------------------------------------------------------*/
/*初始化链表头或者链表结点*/
void list_Init( struct list_head *list );

/*判断链表是否为空或者判断链表结点是否孤立*/
bool list_IsEmpty( struct list_head *list );

/*向链表起始位置添加一个链表结点*/
void list_Add( ListHead_t *head, ListNode_t *node );

/*向链表尾部位置添加一个链表结点*/
void list_AddTail( ListHead_t *head, ListNode_t *node );

/*从链表中移除指定的链表结点*/
void list_Del( ListNode_t *node );

#endif  /* __CPULIB_LIST_H */

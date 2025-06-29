
//
// Created by Quantum on 25-4-13.
//
#include "stdio.h"

#ifndef HEAP_ALLOC_H
#define HEAP_ALLOC_H

#define MIN_HEAP_SIZE (2 * 1024)
#define MAX_HEAP_SIZE (0xFFFF - 1)

#define MIN_SIZE (4)
#define HEAD_NUM (9)

#define GET_INDEX_ADDR(HEAP_PTR, NODE_PTR)                                     \
((uint8_t *)NODE_PTR - (uint8_t *)HEAP_PTR)

#define GET_NODE_PTR(HEAP_PTR, INDEX_ADDR)                                     \
((node_t *)((uint8_t *)HEAP_PTR + INDEX_ADDR))

#define UP_ALIGN(ALIGN_NUM, NUM)                                               \
((((NUM) + ((ALIGN_NUM) - 1)) / (ALIGN_NUM)) * (ALIGN_NUM))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef struct node_t
{
    uint16_t used : 1;          /*!< 表明当前节点内存是否被使用 */
    uint16_t size : 15;         /*!< 表明当前节点的可使用的内存大小，单位：2字节 */
    uint16_t prev;              /*!< 前一节点对应堆内存的索引 */
    uint16_t next;              /*!< 后一节点对应堆内存的索引 */
} node_t;

typedef struct tail_t
{
    uint16_t mem_start;         /*!< 拥有该尾部数据结构的节点对应的堆内存索引 */
} tail_t;

typedef struct heap_t
{
    void* heap_mem;             /*!< 堆维护的内存区起始地址 */
    uint16_t origin_size;       /*!< 维护的内存区大小 */
    uint16_t real_size;         /*!< 实际内存大小 */
    uint16_t heads[9];          /*!< 9个层级链表，管理不同大小范围的内存节点 */
    uint16_t mem_remained;
    uint16_t max_used;
} heap_t;

#define HEAP_FREE_SEC(heap, ptr) \
do{\
heap_free(heap, ptr);\
ptr = (void*)0;\
}while(0)

int heap_init(heap_t* heap, void* heap_mem, int size);
void *heap_alloc(heap_t* heap, size_t size);
void *heap_alloc_nofree(heap_t *heap, size_t origin_size);
int heap_free(heap_t* heap, void* ptr);
void heap_debug(heap_t* heap);
#endif //HEAP_ALLOC_H
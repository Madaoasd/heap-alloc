
//
// Created by Quantum on 25-4-13.
//

#include "heap_alloc.h"
#include "stdio.h"

const uint16_t overhead = sizeof(node_t) + sizeof(tail_t);

/* ------------------------------------------------------------ */
/* -------------------- list operate       -------------------- */
/* ------------------------------------------------------------ */
void remove_node(void* heap_mem, uint16_t* head, node_t* node)
{
    /* 节点是列表中的唯一节点 */
    if (node->next == 0xFFFF && node->prev == 0xFFFF)
    {
        *head = 0xFFFF;
        return;
    }

    /* 节点是列表中的第一个节点，但不唯一 */
    if (node->prev == 0xFFFF)
    {
        uint16_t next = node->next;
        GET_NODE_PTR(heap_mem, next)->prev = 0xFFFF;

        *head = next;
        return;
    }

    /* 节点是列表中的最后一个节点，但不唯一 */
    if (node->next == 0xFFFF)
    {
        uint16_t prev = node->prev;
        GET_NODE_PTR(heap_mem, prev)->next = 0xFFFF;
        return;
    }

    /* 其他情况 */
    GET_NODE_PTR(heap_mem, node->next)->prev = node->prev;
    GET_NODE_PTR(heap_mem, node->prev)->next = node->next;
}

void add_node(void* heap_mem, uint16_t* node_level, node_t* node)
{
    uint16_t cur_addr, pre_addr;
    node->next = 0xFFFF;
    node->prev = 0xFFFF;

    if (*node_level == 0xFFFF)
    {
        *node_level = GET_INDEX_ADDR(heap_mem, node);
        return;
    }

    cur_addr = *node_level;
    pre_addr = 0xFFFF;

    while (cur_addr != 0xFFFF && GET_NODE_PTR(heap_mem, cur_addr)->size <= node->size)
    {
        pre_addr = cur_addr;
        cur_addr = GET_NODE_PTR(heap_mem, cur_addr)->next;
    }

    /* 若到达列表尾部，则将节点链接在尾部 */
    if (cur_addr == 0xFFFF)
    {
        GET_NODE_PTR(heap_mem, pre_addr)->next = GET_INDEX_ADDR(heap_mem, node);
        node->prev = pre_addr;
    }
    else
    {
        /* 将节点链接在第一个 */
        if (pre_addr == 0xFFFF)
        {
            node->next = cur_addr;
            GET_NODE_PTR(heap_mem, cur_addr)->prev = GET_INDEX_ADDR(heap_mem, node);
            *node_level = GET_INDEX_ADDR(heap_mem, node);
        }
        else
        {
            node->next = cur_addr;
            node->prev = pre_addr;

            GET_NODE_PTR(heap_mem, pre_addr)->next = GET_INDEX_ADDR(heap_mem, node);
            GET_NODE_PTR(heap_mem, cur_addr)->prev = GET_INDEX_ADDR(heap_mem, node);
        }
    }
}

/**
 * 根据内存大小获取链表头
 * @param size
 * @return
 */
uint16_t get_head_index(uint16_t size)
{
    uint16_t level_size = MIN_SIZE;
    uint16_t index = 0;

    /* 0 : [4, 8)
     * 1 : [8, 16)
     * 2 : [16, 32)
     *     ...
     *
     */
    for (; index < HEAD_NUM; index++)
    {
        if (level_size <= size)
        {
            level_size <<= 1;
        }
        else
        {
            break;
        }
    }

    return index - 1;
}

/**
 * 获取节点尾部数据结构的地址
 * @param node
 * @return
 */
tail_t *get_tail(node_t *node)
{
    uint16_t offset = (node->size << 1) + sizeof(node_t);
    uint8_t* tail_ptr = (uint8_t*)node + offset;
    return (tail_t *)tail_ptr;
}

/**
 * 对节点尾部数据内容进行配置
 * @param heap_mem
 * @param node
 */
void set_tail(void* heap_mem, node_t* node)
{
    tail_t *tail = get_tail(node);
    tail->mem_start = GET_INDEX_ADDR(heap_mem, node);
}

/**
 * 堆初始化
 * @param heap
 * @param heap_mem
 * @param size
 * @return 若堆的大小超过65535 或小于2K则返回错误。
 */
int heap_init(heap_t* heap, void* heap_mem, int size)
{
    node_t* init_region = (node_t*)heap_mem;
    if (size > MAX_HEAP_SIZE || size < MIN_HEAP_SIZE)
    {
        return -1;
    }

    heap->heap_mem = heap_mem;
    heap->size = size - overhead;
    heap->mem_remained = heap->size;
    heap->max_used = heap->max_used > (heap->size - heap->mem_remained) ?
                     heap->max_used : (heap->size - heap->mem_remained);

    init_region->used = 0;
    init_region->size = ((size - overhead) >> 1);
    set_tail(heap_mem, init_region);

    for (int i = 0; i < HEAD_NUM; i++)
    {
        heap->heads[i] = 0xFFFF;
    }

    uint16_t head_index = get_head_index(init_region->size << 1);
    add_node(heap_mem, &(heap->heads[head_index]), init_region);

    return 1;
}

node_t* get_best_fit_node(void* heap_mem, uint16_t* head, uint16_t size)
{
    uint16_t index_addr;
    if (*head == 0xFFFF)
    {
        return NULL;
    }

    index_addr = *head;
    while (index_addr != 0xFFFF)
    {
        if ((GET_NODE_PTR(heap_mem, index_addr)->size << 1) >= size)
        {
            return GET_NODE_PTR(heap_mem, index_addr);
        }
        index_addr = GET_NODE_PTR(heap_mem, index_addr)->next;
    }

    return NULL;
}

node_t* get_last_node(void* heap_mem, uint16_t* head)
{
    uint16_t index_addr;

    if (*head == 0xFFFF)
    {
        return NULL;
    }

    index_addr = *head;
    while (GET_NODE_PTR(heap_mem, index_addr)->next != 0xFFFF)
    {
        index_addr = GET_NODE_PTR(heap_mem, index_addr)->next;
    }

    return GET_NODE_PTR(heap_mem, index_addr);
}

void* heap_alloc(heap_t* heap, size_t size)
{
    if (size <= 0)
    {
        return NULL;
    }
    size = UP_ALIGN(MIN_SIZE, size);
    uint16_t head = get_head_index(size);
    node_t* find = get_best_fit_node(heap->heap_mem, &(heap->heads[head]), size);

    while (find == NULL)
    {
        head++;
        if (head >= HEAD_NUM)
        {
            return NULL;
        }
        find = get_best_fit_node(heap->heap_mem, &(heap->heads[head]), size);
    }

    if (((find->size << 1) - size) > (overhead + MIN_SIZE))
    {
        node_t* split = (node_t*)((uint8_t*)find + overhead + size);
        split->size = ((find->size << 1) - size - overhead) >> 1;
        split->used = 0;

        set_tail(heap->heap_mem, split);

        uint16_t split_index = get_head_index(split->size << 1);
        add_node(heap->heap_mem, &(heap->heads[split_index]), split);

        find->size = size >> 1;
        set_tail(heap->heap_mem, find);
    }

    find->used = 1;
    remove_node(heap->heap_mem, &(heap->heads[head]), find);
    find->next = 0;
    find->prev = 0;

    heap->mem_remained -= overhead + size;
    heap->max_used = heap->max_used > (heap->size - heap->mem_remained) ?
                     heap->max_used : (heap->size - heap->mem_remained);
    return &(find->next);
}

int check_tail(heap_t *heap, node_t* node)
{
    tail_t *tail = get_tail(node);
    if (tail->mem_start != GET_INDEX_ADDR(heap->heap_mem, node))
    {
        return 1;
    }

    return 0;
}

int check_node_ptr_range(heap_t* heap, void* node)
{
    uint8_t *p = (uint8_t*)node;
    uint8_t *start = (uint8_t*)heap->heap_mem;
    uint8_t *end = start + heap->size;

    if (p >= start && p < end)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int heap_free(heap_t* heap, void* p)
{
    const uint8_t offset = (uint8_t*)(&((node_t*)0)->next) - (uint8_t*)0;

    if ((p == NULL)
        || (check_node_ptr_range(heap, p) != 0))
    {
        return 1;
    }
    node_t *cur_node = (node_t*)((uint8_t*)p - offset);

    if (check_tail(heap, cur_node) != 0)
    {
        return 1;
    }

    heap->mem_remained += overhead + (cur_node->size << 1);
    heap->max_used = heap->max_used > (heap->size - heap->mem_remained) ?
                     heap->max_used : (heap->size - heap->mem_remained);

    uint16_t head_index_addr = GET_INDEX_ADDR(heap->heap_mem, cur_node);
    uint16_t prev_index_addr, next_index_addr;

    /* 这是第一个内存块 */
    if (head_index_addr == 0)
    {
        prev_index_addr = 0xFFFF;
    }
    else
    {
        tail_t *prev_tail = (tail_t*)((uint8_t*)cur_node - sizeof(tail_t));
        prev_index_addr = prev_tail->mem_start;
    }

    /* 这是最后一个内存块 */
    if ((head_index_addr + overhead + (cur_node->size << 1)) >= heap->size)
    {
        next_index_addr = 0xFFFF;
    }
    else
    {
        node_t *next = (node_t*)((uint8_t*)cur_node + overhead + (cur_node->size << 1));
        next_index_addr = GET_INDEX_ADDR(heap->heap_mem, next);
    }

    if (prev_index_addr != 0xFFFF && GET_NODE_PTR(heap->heap_mem, prev_index_addr)->used == 0)
    {
        /* 当前内存块与前一内存块合并 */
        node_t *prev = GET_NODE_PTR(heap->heap_mem, prev_index_addr);
        uint16_t head_index = get_head_index(GET_NODE_PTR(heap->heap_mem, prev_index_addr)->size << 1);
        remove_node(heap->heap_mem, &(heap->heads[head_index]), prev);

        prev->size = (overhead + ((cur_node->size + prev->size) << 1)) >> 1;
        set_tail(heap->heap_mem, prev);

        cur_node = prev;
    }

    if (next_index_addr != 0xFFFF && GET_NODE_PTR(heap->heap_mem, next_index_addr)->used == 0)
    {
        /* 当前内存块与后一内存块合并 */
        node_t *next = GET_NODE_PTR(heap->heap_mem, next_index_addr);
        uint16_t head_index = get_head_index(next->size << 1);
        remove_node(heap->heap_mem, &(heap->heads[head_index]), next);

        cur_node->size = (overhead + ((next->size + cur_node->size) << 1)) >> 1;
        set_tail(heap->heap_mem, cur_node);
    }

    cur_node->used = 0;

    add_node(heap->heap_mem, &(heap->heads[get_head_index(cur_node->size << 1)]), cur_node);

    return 0;
}

void list_show(void *heap_mem, uint16_t head)
{
    uint16_t cur_addr = head;

    while (cur_addr != 0xFFFF)
    {
        printf(" | %d |", GET_NODE_PTR(heap_mem, cur_addr)->size << 1);
        cur_addr = GET_NODE_PTR(heap_mem, cur_addr)->next;
    }
}

void heap_debug(heap_t *heap)
{
    printf("\n\n========== heap debug ==========\n" );
    for (int i=0; i<HEAD_NUM; i++)
    {
        printf("level %d: ", i);
        list_show(heap->heap_mem, heap->heads[i]);
        putchar('\n');
    }
}
#include <stdio.h>
#include "heap_alloc.h"

uint8_t heap_mem[20 * 1024];
heap_t heap;

int size_arr[] = {
    10  , 33, 1000, 665, 10,
    2000, 3 , 500 , 110, 99,
    56  , 90, 9   , 100, 20000,
    2011, 1 , 10  , 50
};

void *ptr_arr[sizeof(size_arr)/sizeof(size_arr[0])];

int main(void)
{
    heap_init(&heap, heap_mem, sizeof(heap_mem));
    heap_debug(&heap);

    int n = sizeof(size_arr)/sizeof(size_arr[0]);
    for (int i = 0; i < n; i++)
    {
        ptr_arr[i] = heap_alloc(&heap, size_arr[i]);
        if (ptr_arr[i] == NULL)
        {
            printf("Failed to allocate memory : %d.\n", i);
        }
    }

    heap_debug(&heap);

    for (int i = n - 1; i >= 0; i--)
    {
        heap_free(&heap, ptr_arr[i]);
    }

    heap_debug(&heap);

    return 0;
}

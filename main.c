
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "heap_alloc.h"
#include "message_subscribe.h"

uint8_t heap_mem[5 * 1024];
heap_t heap;

void *MSG_MALLOC(size_t size)
{
    void* ptr = heap_alloc(&heap, size);

    if (ptr == NULL)
    {
        heap_debug(&heap);
    }
    return ptr;
}

void MSG_FREE(void *ptr)
{
    heap_free(&heap, ptr);
}

MsgBus msg_bus;
MsgObject a;
MsgObject b;
MsgObject c;
MsgObject d;

void a_loop(void);
void b_loop(void);
void c_loop(void);
void d_loop(void);
void time_tick(void);

int main(void)
{
    heap_init(&heap, heap_mem, sizeof(heap_mem));
    heap_debug(&heap);

    msg_bus_init(&msg_bus);
    msg_obj_init(&a);
    msg_obj_init(&b);
    msg_obj_init(&c);
    msg_obj_init(&d);

    msg_obj_subscribe(&msg_bus, &a, "eat");
    msg_obj_subscribe(&msg_bus, &a, "time");

    msg_obj_subscribe(&msg_bus, &b, "sleep");

    msg_obj_subscribe(&msg_bus, &c, "work");

    msg_obj_subscribe(&msg_bus, &d, "drink coffee");
    msg_obj_subscribe(&msg_bus, &d, "work");
    msg_obj_subscribe(&msg_bus, &d, "time");

    printf("heap max used: %d\n", heap.max_used);

    while (1)
    {
        time_tick();
        a_loop();
        b_loop();
        c_loop();
        d_loop();
        msg_destroy();
    }

    return 0;
}

void time_tick(void)
{
    static clock_t last;

    if (((clock() - last)/CLOCKS_PER_SEC) > 0)
    {
        last = clock();
        printf("send time :%d\n", msg_creat_post(&msg_bus, "time", NULL, 0));
        printf("heap max used: %d\n", heap.max_used);
        printf("heap remained: %d\n", heap.mem_remained);
    }
}

void a_loop(void)
{
    static uint8_t num = 1;
    const Msg *msg = msg_obj_fetch_msg(&a);

    while (msg != NULL)
    {
        printf("a get msg: %s\n", msg->msg_name);
        if (strcmp(msg->msg_name, "time") == 0)
        {
            if (num != 0)
            {
                printf( "a send sleep:%d \n",msg_creat_post(&msg_bus, "sleep", NULL, 0));
                num = (num + 1) % 2;
            }
            else
            {
                num = (num + 1) % 2;
                printf( "a send work:%d \n",msg_creat_post(&msg_bus, "work", NULL, 0));
                printf( "a send work:%d \n",msg_creat_post(&msg_bus, "work", NULL, 0));
                printf( "a send work:%d \n",msg_creat_post(&msg_bus, "work", NULL, 0));
                printf( "a send work:%d \n",msg_creat_post(&msg_bus, "work", NULL, 0));
            }
        }
        msg = msg_obj_fetch_msg(&a);
    }

}

void b_loop(void)
{
    const Msg *msg = msg_obj_fetch_msg(&b);
    while (msg != NULL)
    {
        printf("b get msg: %s \n", msg->msg_name);
        msg = msg_obj_fetch_msg(&b);
    }
}

void c_loop(void)
{
    const Msg *msg = msg_obj_fetch_msg(&c);
    static uint8_t work_times = 0;
    while (msg != NULL)
    {
        printf("c get msg: %s\n", msg->msg_name);
        msg = msg_obj_fetch_msg(&c);
        printf("c send eat: %d\n", msg_creat_post(&msg_bus, "eat", NULL, 0));
        work_times = (work_times + 1) % 3;
        if (work_times == 2)
        {
            printf("c send drink coffee: %d\n", msg_creat_post(&msg_bus, "drink coffee", NULL, 0));
        }
    }
}

void d_loop(void)
{
    const Msg *msg = msg_obj_fetch_msg(&d);
    while (msg != NULL)
    {
        printf("d get msg: %s\n", msg->msg_name);
        if (strcmp(msg->msg_name, "time") == 0) {
            printf("\n\nI'm d and I get time message\n");
        }
        msg = msg_obj_fetch_msg(&d);
    }
}
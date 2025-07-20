//
// Created by Quantum on 25-6-14.
//

#include "timer_msg.h"

#include <stdio.h>

typedef struct TimeBus {
    uint64_t tick_ms;
    uint64_t last_tick_ms;
    SingleLinkListHead msg_obj_list;
}TimeBus;

static TimeBus time_bus;
extern void *MSG_MALLOC(size_t size);
extern void MSG_FREE(void *ptr);

void timer_msg_init(TimeMsg *time_msg,
                    const char *msg_name,
                    MsgObject *msg_rev,
                    uint32_t time_interval,
                    uint8_t loop_flag)
{
    time_msg->msg.usr_create_flag = 1;
    time_msg->msg.msg_name = msg_name;

    time_msg->loop_flag = loop_flag;
    time_msg->msg_rev = msg_rev;
    time_msg->time_interval = time_interval;
    time_msg->time_remained = time_interval;
}

int timer_msg_enable(TimeMsg *time_msg)
{
    SingleLinkList *time_node = MSG_MALLOC(sizeof(SingleLinkList));

    timer_msg_disable(time_msg);

    if (time_node == NULL) {
        return -1;
    }

    single_link_node_init(time_node, (void*)time_msg);

    time_msg->time_remained = time_msg->time_interval;
    single_link_list_add_head(&time_bus.msg_obj_list, time_node);

    return 0;
}

int timer_msg_disable(TimeMsg *time_msg)
{
    SingleLinkList *time_node = single_link_list_find_time_msg(&time_bus.msg_obj_list, time_msg);

    if (time_node != NULL) {
        single_link_list_del(&time_bus.msg_obj_list, time_node);
    }

    MSG_FREE(time_node);

    return 0;
}

int timer_interrupt_handle()
{
    time_bus.tick_ms += 1;
}

int timer_thread_handle()
{
    SingleLinkList *time_node = time_bus.msg_obj_list;

    if (!(time_bus.tick_ms > time_bus.last_tick_ms)) {
        return 0;
    }

    uint16_t diff = time_bus.tick_ms - time_bus.last_tick_ms;

    while (time_node != NULL) {

        TimeMsg *time_msg = (TimeMsg *)time_node->data;

        if (time_msg->time_remained <= diff) {
            msg_post_to_obj(&time_msg->msg, time_msg->msg_rev);
            time_msg->time_remained = time_msg->time_interval;
            if (!time_msg->loop_flag) {
                SingleLinkList *temp = time_node;
                time_node = time_node->next;
                single_link_list_del(&time_bus.msg_obj_list, temp);
                MSG_FREE(temp);
                continue;
            }
        }
        else {
            time_msg->time_remained -= diff;
        }

        time_node = time_node->next;
    }

    time_bus.last_tick_ms = time_bus.tick_ms;
}


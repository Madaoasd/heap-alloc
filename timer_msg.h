//
// Created by Quantum on 25-6-14.
//

#ifndef TIMER_H
#define TIMER_H
#include "subscribe_msg.h"

typedef struct TimeMsg{
    Msg msg;
    uint8_t loop_flag;
    uint32_t time_interval;
    uint32_t time_remained;
    MsgObject *msg_rev;
}TimeMsg;

void timer_msg_init(TimeMsg *time_msg,
                    const char *msg_name,
                    MsgObject *msg_rev,
                    uint32_t time_interval,
                    uint8_t loop_flag);

int timer_msg_enable(TimeMsg *time_msg);

int timer_msg_disable(TimeMsg *time_msg);

int timer_interrupt_handle();

int timer_thread_handle();
#endif //TIMER_H

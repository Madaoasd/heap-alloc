
//
// Created by Administrator on 25-4-17.
//

#include "message_subscribe.h"

#include <stdio.h>
#include <string.h>

struct MsgDestroyPool
{
    Msg* msgs_to_free[20]; /*!< 待销毁的消息 */
};

static struct MsgDestroyPool g_msg_destroy_pool = {0};

#define DIM_OF(array) (sizeof(array) / sizeof(array[0]))

void* MSG_MALLOC(size_t size) __attribute__((weak));
void MSG_FREE(void* ptr) __attribute__((weak));

int msg_bus_init(MsgBus* msg_bus)
{
    msg_bus->msg_obj_list = (void*)0;

    return 0;
}

int msg_obj_init(MsgObject* msg_obj)
{
    msg_obj->msg_str_list = NULL;

    for (int i = 0; i < DIM_OF(msg_obj->msgs); i++)
    {
        msg_obj->msgs[i] = NULL;
    }

    msg_obj->msg_idx = 0;

    return 0;
}

int msg_obj_bind_name(MsgObject* msg_obj, const char* msg_name)
{
    SingleLinkList *node = single_link_list_find_str(&msg_obj->msg_str_list, msg_name);

    if (node != NULL)
    {
        return 0;
    }

    node = MSG_MALLOC(sizeof(SingleLinkList));

    if (node == NULL)
    {
        return -1;
    }

    single_link_node_init(node, msg_name);

    single_link_list_add_head(&msg_obj->msg_str_list, node);

    /* 无空位 */
    return 0;
}

static int is_obj_subscribe_name(MsgObject* msg_obj, const char* msg_name)
{
    SingleLinkList *node = single_link_list_find_str(&msg_obj->msg_str_list, msg_name);

    if (node == NULL)
    {
        return -1;
    }

    return 0;
}

int msg_obj_bind_msg(MsgObject* msg_obj, Msg* msg)
{
    for (int i = 0; i < DIM_OF(msg_obj->msgs); i++)
    {
        /* 若已注册直接返回，没有注册则在空位注册 */
        if (msg_obj->msgs[i] == NULL)
        {
            msg_obj->msgs[i] = msg;
            return 0;
        }
        else if (msg_obj->msgs[i] == msg)
        {
            return 0;
        }
    }

    /* 无空位 */
    return -1;
}

int msg_obj_subscribe(MsgBus* msg_bus, MsgObject* msg_obj, const char* msg_name)
{
    SingleLinkList *obj_node = single_link_list_find_msg_obj(&msg_bus->msg_obj_list, msg_obj);

    if (obj_node != NULL)
    {
        return msg_obj_bind_name(msg_obj, msg_name);
    }

    obj_node = MSG_MALLOC(sizeof(SingleLinkList));

    if (obj_node == NULL)
    {
        return -1;
    }

    single_link_node_init(obj_node, msg_obj);
    single_link_list_add_head(&msg_bus->msg_obj_list, obj_node);

    return msg_obj_bind_name(msg_obj, msg_name);
}

static int msg_mark_destroy(Msg* msg)
{
    for (int i = 0; i < DIM_OF(g_msg_destroy_pool.msgs_to_free); i++)
    {
        if (g_msg_destroy_pool.msgs_to_free[i] == NULL)
        {
            g_msg_destroy_pool.msgs_to_free[i] = msg;
            return 0;
        }
        else if (g_msg_destroy_pool.msgs_to_free[i] == msg)
        {
            return 0;
        }
    }

    return -1;
}

const Msg* msg_obj_fetch_msg(MsgObject* msg_obj)
{
    Msg* ret = NULL;

    for (int i = 0; i < DIM_OF(msg_obj->msgs); i++)
    {
        uint8_t temp_idx = (msg_obj->msg_idx + i) % DIM_OF(msg_obj->msgs);

        if (msg_obj->msgs[temp_idx] != NULL)
        {
            ret = msg_obj->msgs[temp_idx];
            msg_obj->msgs[temp_idx] = NULL;

            ret->ref_count -= 1;

            msg_obj->msg_idx = (temp_idx + 1) % DIM_OF(msg_obj->msgs);

            if (ret->ref_count == 0 && ret->usr_create_flag == 0)
            {
                msg_mark_destroy(ret);
            }

            return ret;
        }
    }

    return ret;
}

int msg_occupy(Msg* msg)
{
    msg->ref_count += 1;
}

int msg_remove_occupy(Msg* msg)
{
    if (msg->ref_count > 0)
    {
        msg->ref_count -= 1;
    }

    if (msg->ref_count == 0
        && msg->usr_create_flag == 0)
    {
        return msg_mark_destroy(msg);
    }
    return 0;
}

void msg_destroy(void)
{
    for (int i = 0; i < DIM_OF(g_msg_destroy_pool.msgs_to_free); i++)
    {
        if (g_msg_destroy_pool.msgs_to_free[i] != NULL)
        {
            MSG_FREE(g_msg_destroy_pool.msgs_to_free[i]);
            g_msg_destroy_pool.msgs_to_free[i] = NULL;
        }
    }
}

int msg_creat_post(MsgBus* msg_bus, const char* msg_name)
{
    SingleLinkList *obj_node = NULL;
    uint8_t ref_count = 0;
    int i;

    if (msg_name == NULL)
    {
        printf("msg_name error\n");
        return -1;
    }

    obj_node = msg_bus->msg_obj_list;
    while (obj_node != NULL)
    {
        MsgObject *obj = (MsgObject*)obj_node->data;
        if (is_obj_subscribe_name(obj, msg_name) >= 0)
        {
            ref_count = 1;
            break;
        }
        obj_node = obj_node->next;
    }

    if (!ref_count)
    {
        printf("ref_count error\n");
        return -1;
    }

    Msg* msg = MSG_MALLOC(sizeof(Msg));
    if (msg == NULL)
    {
        printf("malloc error\n");
        return -1;
    }

    msg->msg_name = msg_name;

    ref_count = 0;

    obj_node = msg_bus->msg_obj_list;
    while (obj_node != NULL)
    {
        MsgObject* obj = (MsgObject*)obj_node->data;
        if (is_obj_subscribe_name(obj, msg_name) >= 0)
        {
            ref_count += 1;
            msg_obj_bind_msg(obj, msg);
        }
        obj_node = obj_node->next;
    }

    msg->ref_count = ref_count;
    return 0;
}

int msg_post(MsgBus* msg_bus, Msg* msg)
{
    msg->usr_create_flag = 1;
    msg->ref_count = 0;

    SingleLinkList *obj_node = msg_bus->msg_obj_list;
    while (obj_node != NULL)
    {
        MsgObject *obj = (MsgObject*)obj_node->data;
        if (is_obj_subscribe_name(obj, msg->msg_name) >= 0)
        {
            msg_obj_bind_msg(obj, msg);
            msg->ref_count += 1;
        }
        obj_node = obj_node->next;
    }

}

int msg_post_to_obj(Msg* msg, MsgObject* msg_obj)
{
    if (msg == NULL || msg_obj == NULL)
    {
        return -1;
    }

    return msg_obj_bind_msg(msg_obj, msg);
}
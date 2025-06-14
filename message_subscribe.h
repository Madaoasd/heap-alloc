
//
// Created by Administrator on 25-4-17.
//
#ifndef  __MESSAGE_SUBSCRIPTION_H
#define  __MESSAGE_SUBSCRIPTION_H

#include <stdint.h>
#include "SingleLinkList.h"
typedef struct MsgBus MsgBus;
typedef struct MsgObject MsgObject;
typedef struct Msg Msg;

struct MsgBus
{
    SingleLinkListHead msg_obj_list;
};

struct MsgObject
{
    SingleLinkListHead msg_str_list;
    Msg *msgs[10];                      /*!< 接收到的消息 */
    uint8_t msg_idx;
};

struct Msg
{
    uint8_t ref_count;                  /*!< 被引用数 */
    uint8_t usr_create_flag;            /*!< 被用户创建的消息 */
    const char *msg_name;               /*!< 消息名称 */
    void *msg_content;                  /*!< 消息内容 */
    int content_len;                    /*!< 内容长度 */
};

/**
 * 消息总线初始化（将成员初始化为0）
 * @param msg_bus
 * @return
 */
int msg_bus_init(MsgBus* msg_bus);

/**
 * 消息对象初始化（将成员初始为0）
 * @param msg_obj
 * @return
 */
int msg_obj_init(MsgObject *msg_obj);

/**
 * 向全局消息组件注册接收对象以及目标消息
 * @param msg_object 消息对象
 * @param msg_name 消息名
 * @return
 */
int msg_obj_subscribe(MsgBus *msg_bus, MsgObject *msg_obj, const char *msg_name);

/**
 * 取出绑定到该消息对象的消息
 * @param msg_object
 * @return
 */
const Msg* msg_obj_fetch_msg(MsgObject *msg_obj);

/**
 * 占有消息(避免被总线销毁)
 * @param msg
 * @return
 */
int msg_occupy(Msg *msg);

/**
 * 取消消息占有
 * @param msg
 * @return
 */
int msg_remove_occupy(Msg *msg);

/**
 * 清除总线创建的废弃消息（堆内存释放）
 */
void msg_destroy(void);

/**
 * 发布消息: 消息由总线创建
 * @param msg_bus
 * @param msg_name
 * @param msg_content
 * @param content_len
 */
int msg_creat_post(MsgBus* msg_bus, const char* msg_name, void* msg_content, int content_len);

/**
 * 发布消息: 消息由用户创建
 * @param msg_bus
 * @param msg
 * @return
 */
int msg_post(MsgBus* msg_bus, Msg *msg);

/**
 *
 * @param msg
 * @param msg_obj
 * @return
 */
int msg_post_to_obj(Msg *msg, MsgObject *msg_obj);

#endif
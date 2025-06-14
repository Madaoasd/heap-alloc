
//
// Created by Administrator on 25-6-13.
//

#include "SingleLinkList.h"

#include <stdio.h>
#include <string.h>

int single_link_node_init(SingleLinkList *node, const void* data)
{
    node->data = data;
    node->next = (void*)0;

    return 0;
}

int single_link_list_add_tail(SingleLinkListHead *head, SingleLinkList *node)
{
    SingleLinkList *temp_node = *head;

    if (temp_node == (void*)0)
    {
        *head = node;
        return 0;
    }

    while (temp_node->next != (void*)0)
    {
        temp_node = temp_node->next;
    }
    temp_node->next = node;

    return 0;
}

int single_link_list_add_head(SingleLinkListHead *head, SingleLinkList *node)
{
    SingleLinkList *temp_node = *head;

    if (temp_node == (void*)0)
    {
        *head = node;
    }

    node->next = temp_node;

    *head = node;

    return 0;
}

SingleLinkList* single_link_list_fetch_head(SingleLinkListHead* head)
{
    SingleLinkList* temp_node = *head;

    if (temp_node == (void*)0)
    {
        return NULL;
    }

    *head = temp_node->next;
    temp_node->next = (void*)0;

    return temp_node;
}

int single_link_list_del(SingleLinkListHead *head, SingleLinkList *node)
{
    SingleLinkList *temp_node = *head;

    if (temp_node == (void*)0)
    {
        return -1;
    }

    if (temp_node == node)
    {
        *head = temp_node->next;
        node->next = (void*)0;
        return 0;
    }

    while (temp_node->next != node)
    {
        temp_node = temp_node->next;
    }

    if (temp_node->next != (void*)0)
    {
        temp_node->next = node->next;
        node->next = (void*)0;
        return 0;
    }
    else
    {
        return -1;
    }
}

SingleLinkList* single_link_list_find_str(SingleLinkListHead* head, const char* str)
{
    SingleLinkList* temp_node = *head;

    while (temp_node != (void*)0)
    {
        if (temp_node->data != (void*)0
            && strcmp(temp_node->data, str) == 0)
        {
            return temp_node;
        }
        temp_node = temp_node->next;
    }

    return (void*)0;
}

SingleLinkList* single_link_list_find_msg_obj(SingleLinkListHead* head, struct MsgObject* obj)
{
    SingleLinkList* temp_node = *head;

    while (temp_node != (void*)0)
    {
        if (temp_node->data != (void*)0
            && (struct MsgObject*)temp_node->data == obj)
        {
            return temp_node;
        }
        temp_node = temp_node->next;
    }

    return (void*)0;
}

//
// int main(void)
// {
//     SingleLinkList a, b, c, d;
//
//     single_link_list_init(&a, "hello");
//     single_link_list_init(&b, "nihao");
//     single_link_list_init(&c, "goodbye");
//     single_link_list_init(&d, "zaijian");
//
//     SingleLinkListHead head = (void*)0;
//     single_link_list_add_tail(&head, &a);
//     single_link_list_add_tail(&head, &b);
//     single_link_list_add_tail(&head, &c);
//     single_link_list_add_tail(&head, &d);
//
//     SingleLinkList *temp_node = head;
//     while (temp_node != (void*)0)
//     {
//         printf("%s\n", (char*)temp_node->data);
//         temp_node = temp_node->next;
//     }
//
//     temp_node = single_link_list_find_str(&head, "zaijian");
//     if (temp_node != (void*)0)
//     {
//         printf("%s\n", (char*)temp_node->data);
//     }
//     else
//     {
//         printf("find failed\n");
//     }
//
//     single_link_list_del(&head, &d);
//     single_link_list_del(&head, &c);
//     single_link_list_del(&head, &b);
//     single_link_list_del(&head, &a);
//
//     temp_node = head;
//     while (temp_node != (void*)0)
//     {
//         printf("%s\n", (char*)temp_node->data);
//         temp_node = temp_node->next;
//     }
//
//     single_link_list_add_head(&head, &a);
//     single_link_list_add_head(&head, &b);
//     single_link_list_add_head(&head, &c);
//     single_link_list_add_head(&head, &d);
//     temp_node = head;
//     while (temp_node != (void*)0)
//     {
//         printf("%s\n", (char*)temp_node->data);
//         temp_node = temp_node->next;
//     }
// }
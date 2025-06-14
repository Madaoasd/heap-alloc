
//
// Created by Administrator on 25-6-13.
//

#ifndef LIST_H
#define LIST_H

struct SingleLinkList;
typedef struct SingleLinkList
{
    struct SingleLinkList *next;
    const void *data;
} SingleLinkList;

typedef SingleLinkList* SingleLinkListHead;

int single_link_node_init(SingleLinkList* node, const void* data);
int single_link_list_add_tail(SingleLinkListHead* head, SingleLinkList* node);
int single_link_list_add_head(SingleLinkListHead* head, SingleLinkList* node);
SingleLinkList* single_link_list_fetch_head(SingleLinkListHead* head);
int single_link_list_del(SingleLinkListHead* head, SingleLinkList* node);
SingleLinkList* single_link_list_find_str(SingleLinkListHead* head, const char* str);

struct MsgObject;
SingleLinkList* single_link_list_find_msg_obj(SingleLinkListHead* head, struct MsgObject* obj);

#endif //LIST_H
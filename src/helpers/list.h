#ifndef LIST_HEADER
#define LIST_HEADER

#include <stdlib.h>

/* TODO: Change other headers functions to be in this structure, (... list_func(list *l, ...) */
typedef struct list_node
{
    struct list_node *next;
    void *data;
} list_node;

typedef struct
{
    list_node *head;
    list_node *tail;
} list;

list *list_new();
void list_add(list *l, void *data);
void *list_get_next(list *l);
void list_free(list *l);

#endif
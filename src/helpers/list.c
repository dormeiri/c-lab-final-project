#include "list.h"

List *list_new(size_t size)
{
    List *result;
    
    if(!(result = (List *)malloc(sizeof(List))))
    {
        exit(EXIT_FAILURE);
    }
    result->head = NULL;
    result->tail = NULL;

    return result;
}

void list_add(List *l, void *data)
{
    ListNode *new;
    if(!(new = (ListNode *)malloc(sizeof(ListNode))))
    {
        exit(EXIT_FAILURE);
    }

    new->data = data;
    new->next = NULL;

    if(l->tail)
    {
        l->tail->next = new;
        l->tail = l->tail->next;
    }
    else
    {
        l->tail = l->head = new;
    }
}

void *list_get_next(List *l)
{
    static List *l_prev = NULL;
    static ListNode *n_prev = NULL;

    void *result;

    if(l == NULL) /* To be able to rewind without using different list */
    {
        return NULL;
    }

    if(l != l_prev)
    {
        l_prev = l;
        n_prev = l_prev->head;
    }

    if(n_prev)
    {
        result = n_prev->data;
        n_prev = n_prev->next;
    }
    else
    {
        l_prev = NULL;
        n_prev = NULL;
        result = NULL;
    }

    return result;
}

void list_free(List *l)
{
    ListNode *temp;
    while((temp = l->head))
    {
        l->head = l->head->next;
        free(temp);
    }
    free(l);
}
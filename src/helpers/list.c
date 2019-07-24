#include "list.h"

list *list_new(size_t size)
{
    list *result;
    
    if(!(result = (list *)malloc(sizeof(list))))
    {
        exit(EXIT_FAILURE);
    }
    result->head = NULL;
    result->tail = NULL;

    return result;
}

void list_add(list *l, void *data)
{
    list_node *new;
    if(!(new = (list_node *)malloc(sizeof(list_node))))
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

void *list_get_next(list *l)
{
    static list *l_prev = NULL;
    static list_node *n_prev = NULL;

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

void list_free(list *l)
{
    list_node *temp;
    while((temp = l->head))
    {
        l->head = l->head->next;
        free(temp);
    }
}
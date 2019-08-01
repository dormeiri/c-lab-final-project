/* 
*   Documenting:        DONE
*   Naming:             DONE
*   Code complexity:    DONE
*   Unused check:       DONE
*   Encapsulation:      DONE
*   Order:              DONE
*/

#include "list.h"

/* Allocate new memory for List and initialize its head and tail */
List *list_new(size_t size)
{
    List *result; /* The new created List */
    
    if(!(result = (List *)malloc(sizeof(List))))
    {
        exit(EXIT_FAILURE);
    }
    result->head = NULL;
    result->tail = NULL;

    return result;
}

/* Allocate new memory for ListNode and add the node to the end of the list */
void list_add(List *l, void *data)
{
    ListNode *new; /* The new added node */
    if(!(new = (ListNode *)malloc(sizeof(ListNode))))
    {
        exit(EXIT_FAILURE);
    }

    new->data = data;
    new->next = NULL;

    /* If the tail is not NULL then add the new node after the tail and make it to be the tail */
    if(l->tail)
    {
        l->tail->next = new;
        l->tail = l->tail->next;
    }
    else /* Otherwise, that mean that this is the first node in the list */
    {
        l->tail = l->head = new;
    }
}

/* Get the next node of the current list stored do that until new list inserted or until the end of the list  or until NULL inserted*/
void *list_get_next(List *l)
{
    static List *l_prev = NULL;     /* The previous list accepted as parameter to check whether continue with that list or start new iteration */
    static ListNode *n_prev = NULL; /* The last node accepted as parameter so we will know what node will be next to return */

    void *result;   /* the data of the next node */

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

/* Go throught the nodes in the list and free it, after that free the list itself */
void list_free(List *l)
{
    ListNode *temp; /* Temp node to free after updating head */
    while((temp = l->head))
    {
        l->head = l->head->next;
        free(temp);
    }
    free(l);
}
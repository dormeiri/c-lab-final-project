/* TODO:    Change other headers to be like this header 
*           Header functions prototypes should be like following: X list_func(list *l, ...)
*           Comments should be like following:
*\tDescription:\t...
*\tParams:
*\t\t-X:\t...
*\t\t-Y:\t...
*\tReturn:\t...
*/

#ifndef LIST_HEADER
#define LIST_HEADER

#include <stdlib.h>

/* Generic list node */
typedef struct ListNode
{
    struct ListNode *next; /* The next node in the list */
    void *data;             /* The generic data of the node */
} ListNode;

/* Generic list */
typedef struct
{
    ListNode *head;        /* The head of the list */
    ListNode *tail;        /* The tail of the list */
} List;

/*  Description:    Allocate memory and initialize new empty list
*   Return:         New list
*/
List *list_new();

/*  Description:    Create new generic list node with the given data and add it to the end of the list
*   Params:
*       - l:        Add the new node to this list
*       - data:     Allocate and create new node with this data
*/
void list_add(List *l, void *data);

/*  Description:    Get the next data of the given list, 
*                   the function store the last given list 
*                   and use it to inidicate wheter to start new iteration or continue from the previous node returned.
*                   If NULL is inserted, the iteration will be restarted
*   Params:
*       - l:        Get the next node from this list. Insert each time the same list to continue iteration. 
*                   restart with inserting NULL or different list.
*   Return:         The generic data of the next node in the list.
*/
void *list_get_next(List *l);

/*  Description:    Free all of the nodes of the list and the list itself
*   Params:
*       - l:        The list to be freed
*/
void list_free(List *l);

#endif
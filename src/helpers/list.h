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
typedef struct list_node
{
    struct list_node *next; /* The next node in the list */
    void *data;             /* The generic data of the node */
} list_node;

/* Generic list */
typedef struct
{
    list_node *head;        /* The head of the list */
    list_node *tail;        /* The tail of the list */
} list;

/*  Description:    Allocate memory and initialize new empty list
*   Return:         New list
*/
list *list_new();

/*  Description:    Create new generic list node with the given data and add it to the end of the list
*   Params:
*       - l:        Add the new node to this list
*       - data:     Allocate and create new node with this data
*/
void list_add(list *l, void *data);

/*  Description:    Get the next data of the given list, 
*                   the function store the last given list 
*                   and use it to inidicate wheter to start new iteration or continue from the previous node returned.
*                   If NULL is inserted, the iteration will be restarted
*   Params:
*       - l:        Get the next node from this list. Insert each time the same list to continue iteration. 
*                   restart with inserting NULL or different list.
*   Return:         The generic data of the next node in the list.
*/
void *list_get_next(list *l);

/*  Description:    Free all of the nodes of the list and the list itself
*   Params:
*       - l:        The list to be freed
*/
void list_free(list *l);

#endif
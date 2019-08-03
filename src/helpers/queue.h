#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdlib.h>
#include "../commons.h"

/* Generic queue node */
typedef struct QueueNode
{
    void *data;
    struct QueueNode *next;
} QueueNode;

/* Generic queue */
typedef struct Queue
{
    QueueNode *head;
    QueueNode *tail;
    size_t data_size;
} Queue;

#define IS_EMPTY_QUEUE(QREF) (!(QREF) || (QREF)->head == NULL)

/*  Description:    Initialize a new queue
*   Parameters:
*       node_size:  The size of each node in the queue
*   Return:         The head of the initialized queue
*/
Queue *queue_new(size_t node_size);

/*  Description:    Enqueue new node into queue
*   Params:
*       queue:      The head of the queue
*       data:       Reference to a generic value
*/
void enqueue(Queue *q, void *data);

/*  Description:    Get the next value of queue and free the queue node, pay attention: this does not remove nested pointers of structs
*   Params:
*       - queue:    Pointer to a queue to dequeue
*   Return:         The value of the dequeued node
*/
void *dequeue(Queue *q);

/*  Description:    Free queue, pay attention: this does not remove nested pointers of structs.
*   Params:
*       - queue:    Pointer to the queue to free
*/
void queue_free(Queue *q);

#endif
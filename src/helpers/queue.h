#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdlib.h>
#include "../commons.h"

typedef struct queue_node
{
    void *data;
    struct queue_node *next;
} queue_node;

typedef struct queue
{
    queue_node *head;
    queue_node *tail;
    size_t data_size;
} queue;

#define IS_EMPTY_QUEUE(QREF) (QREF->head == NULL)

/*
Description:    Initialize a new queue
Parameters:
    node_size:  The size of each node in the queue
Return:         The head of the initialized queue
Exceptions:     Exit if couldn't allocate memory for the queue
*/
queue *initilize_queue(size_t node_size);

/*
Description:    Enqueue new node into queue
Parameters:
    queue:      The head of the queue
    value_ref:  Reference to a generic value
Exceptions:     Exit if couldn't allocate memory for the queue
*/
void enqueue(queue *queue, void *value_ref);

/*
Description:    Dequeue tail from queue
Parameters:
    queue:      Pointer to a queue
Return:         The value of the dequeued node
*/
void *dequeue(queue *queue);

#endif
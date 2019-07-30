#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include <stdlib.h>
#include "../commons.h"

typedef struct QueueNode
{
    void *data;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *head;
    QueueNode *tail;
    size_t data_size;
} Queue;

#define IS_EMPTY_QUEUE(QREF) (!QREF || (QREF)->head == NULL)

/*
Description:    Initialize a new queue
Parameters:
    node_size:  The size of each node in the queue
Return:         The head of the initialized queue
Exceptions:     Exit if couldn't allocate memory for the queue
*/
Queue *initilize_queue(size_t node_size);

/*
Description:    Enqueue new node into queue
Parameters:
    queue:      The head of the queue
    data:       Reference to a generic value
Exceptions:     Exit if couldn't allocate memory for the queue
*/
void enqueue(Queue *q, void *data);

/*
Description:    Dequeue tail from queue
Parameters:
    queue:      Pointer to a queue
Return:         The value of the dequeued node
*/
void *dequeue(Queue *q);

/*
Description:    Free queue
Parameters:
    queue:      Pointer to a queue
*/
void free_queue(Queue *q);

#endif
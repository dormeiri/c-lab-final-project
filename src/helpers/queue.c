#include <string.h>
#include "queue.h"

Queue *queue_new(size_t data_size)
{
    Queue *new_queue = (Queue *)malloc(sizeof(Queue));
    new_queue->head = NULL;
    new_queue->tail = NULL;
    new_queue->data_size = data_size;

    return new_queue;
}

void queue_free(Queue *Queue)
{
    QueueNode *temp;
    while((temp = Queue->head))
    {
        Queue->head = Queue->head->next;
        free(temp);
    }
}

void enqueue(Queue *Queue, void *value_ref)
{
    /* Allocate memory for new node */
    QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
    if(new_node == NULL)
    {
        exit(EXIT_FAILURE);
    }

    new_node->data = value_ref;
    new_node->next = NULL;

    /* Add the new node to the queue */
    if(IS_EMPTY_QUEUE(Queue))
    {
        Queue->head = Queue->tail = new_node;
    }
    else
    {
        Queue->tail = Queue->tail->next = new_node;
    }
}

void *dequeue(Queue *Queue)
{
    QueueNode *removed_node;
    void *value_ref;

    if(IS_EMPTY_QUEUE(Queue))
    {
        return NULL;
    }

    if(!(value_ref = malloc(Queue->data_size)))
    {
        exit(EXIT_FAILURE);
    }

    removed_node = Queue->head;
    /* Copy the data from the node to local variable because we are going to free the node */
    memcpy(value_ref, Queue->head->data, Queue->data_size);

    Queue->head = Queue->head->next;
    if(!Queue->head)
    {
        Queue->tail = NULL;
    }

    free(removed_node);

    return value_ref;
}

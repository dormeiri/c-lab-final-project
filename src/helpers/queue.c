#include <string.h>
#include "queue.h"

queue *initilize_queue(size_t data_size)
{
    queue *new_queue = (queue *)malloc(sizeof(queue));
    new_queue->head = NULL;
    new_queue->tail = NULL;
    new_queue->data_size = data_size;

    return new_queue;
}

void enqueue(queue *queue, void *value_ref)
{
    /* Allocate memory for new node */
    queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
    if(new_node == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for the new node's data (generic type) */
    new_node->data = malloc(queue->data_size);
    if(new_node->data == NULL)
    {
        exit(EXIT_FAILURE);
    }

    new_node->data = value_ref;

    /* Add the new node to the queue */
    if(IS_EMPTY_QUEUE(queue))
    {
        queue->tail = new_node;
        queue->head = queue->tail;
    }
    else
    {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
}

void *dequeue(queue *queue)
{
    queue_node *removed_node = queue->head;
    void *value_ref = malloc(queue->data_size);

    if(value_ref == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* Copy the data from the node to local variable because we are going to free the node */
    memcpy(value_ref, queue->head->data, queue->data_size);

    if(queue->head->next)
    {
        queue->head = removed_node->next;
    }
    else
    {
        queue->head = NULL;
        queue->tail = NULL;
    }

    free(removed_node);

    return value_ref;
}

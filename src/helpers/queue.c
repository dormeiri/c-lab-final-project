#include <stdio.h>
#include "queue.h"

queue *initilize_queue(size_t node_size)
{
    queue *new_queue = (queue *)malloc(sizeof(queue));
    new_queue->head = NULL;
    new_queue->tail = NULL;

    return new_queue;
}

void enqueue(queue *queue, void *value)
{
    /* Allocate memory for new node */
    queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
    if(new_node == NULL)
    {
        exit(EXIT_FAILURE); /* TODO: Should we free before? */
    }

    /* Allocate memory for the new node's data (generic type) */
    new_node->data = malloc(queue->node_size);
    if(new_node->data == NULL)
    {
        exit(EXIT_FAILURE); /* TODO: should we free before? */
    }

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
    void *value = queue->head->data;

    if(queue->head->next)
    {
        queue->head = removed_node->next;
    }
    else
    {
        queue->head = NULL;
        queue->tail = NULL;
    }

    /* TODO: Will cause error because value is part of removed_node, need to think how we free it without harming variable value */
    /* free(removed_node); */

    return value;
}

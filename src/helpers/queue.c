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

void free_queue(queue *queue)
{
    queue_node *temp;
    while((temp = queue->head))
    {
        queue->head = queue->head->next;
        free(temp);
    }
}

void enqueue(queue *queue, void *value_ref)
{
    /* Allocate memory for new node */
    queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
    if(new_node == NULL)
    {
        exit(EXIT_FAILURE);
    }

    new_node->data = value_ref;
    new_node->next = NULL;

    /* Add the new node to the queue */
    if(IS_EMPTY_QUEUE(queue))
    {
        queue->head = queue->tail = new_node;
    }
    else
    {
        queue->tail = queue->tail->next = new_node;
    }
}

void *dequeue(queue *queue)
{
    queue_node *removed_node;
    void *value_ref;

    if(IS_EMPTY_QUEUE(queue))
    {
        return NULL;
    }

    if(!(value_ref = malloc(queue->data_size)))
    {
        exit(EXIT_FAILURE);
    }

    removed_node = queue->head;
    /* Copy the data from the node to local variable because we are going to free the node */
    memcpy(value_ref, queue->head->data, queue->data_size);

    queue->head = queue->head->next;
    if(!queue->head)
    {
        queue->tail = NULL;
    }

    free(removed_node);

    return value_ref;
}

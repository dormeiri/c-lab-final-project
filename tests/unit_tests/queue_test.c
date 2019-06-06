#include "unit_test.h"
#include "../../src/helpers/queue.h"

void enqueue_test(queue *q, long expected);
void dequeue_test(queue *q, long expected);
void is_empty_queue_test(queue *q);
void same_queue_head_tail_test(queue *q);

int main()
{
    long expected = 32L;

    queue *q = initilize_queue(sizeof(expected));

    is_empty_queue_test(q);
    enqueue_test(q, expected);

    same_queue_head_tail_test(q);

    dequeue_test(q, expected);
    is_empty_queue_test(q);

    enqueue_test(q, 7L);
    same_queue_head_tail_test(q);
    enqueue_test(q, 42L);
    enqueue_test(q, 70L);
    
    dequeue_test(q, 7L);
    dequeue_test(q, 42L);
    same_queue_head_tail_test(q);
    dequeue_test(q, 70L);

    is_empty_queue_test(q);

    return 0;
}

void enqueue_test(queue *q, long expected)
{
    static char *name = "enqueue_test";
    long actual;

    enqueue(q, &expected);

    actual = *(long *)q->head->data;

    assert(name, &actual, &expected, LONG);
}

void dequeue_test(queue *q, long expected)
{
    static char *name = "dequeue_test";
    long actual = *(long *)dequeue(q);

    assert(name, &actual, &expected, LONG);
}

void is_empty_queue_test(queue *q)
{
    static char *name = "is_empty_queue_test";
    int expected = TRUE;
    int actual = IS_EMPTY_QUEUE(q);

    assert(name, &actual, &expected, INT);

    same_queue_head_tail_test(q);
}

void same_queue_head_tail_test(queue *q)
{
    static char *name = "same_queue_head_tail_test";

    queue_node *node_expected_ref = q->head;
    queue_node *node_actual_ref = q->tail;
    assert(name, &node_actual_ref, &node_expected_ref, LONG);
}
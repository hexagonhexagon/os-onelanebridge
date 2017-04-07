typedef struct _queue
{
    int numitems;
    int queue[20];
    int head;
    int back;
} queue;

void queue_enqueue(queue *q, int item)
{
    q->queue[q->back] = item;
    q->back = (q->back + 1) % 20;
    q->numitems++;
}

void queue_dequeue(queue *q)
{
    q->head = (q->head + 1) % 20;
    q->numitems--;
}

int queue_peek(queue *q)
{
    return q->queue[q->head];
}

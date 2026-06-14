#ifndef QUEUE_H
#define QUEUE_H
#include "uthreads.h"

typedef struct {
    tcb* head;
    tcb* tail;
} queue;

queue* queue_init();
void enqueue(queue* q, tcb* thread);
tcb* dequeue(queue* q);
tcb* front(queue* q);
void freeQueue(queue* q);
void empty_queue(queue* q);

#endif
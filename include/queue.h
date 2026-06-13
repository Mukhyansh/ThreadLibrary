#ifndef QUEUE_H
#define QUEUE_H

typedef struct{
    void* arr[8096];
    int front;
    int end;
}queue;

queue* init();
void push(queue* q,void* ch);
void* pop(queue* q);
void* front(queue* q);
void freeQueue(queue* q);

#endif
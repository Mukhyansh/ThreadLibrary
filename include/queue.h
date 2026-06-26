#ifndef QUEUE_H
#define QUEUE_H

#include"uthreads.h"

typedef struct queue queue;
typedef struct umutex umutex;

typedef enum{
    READY=0,
    RUNNING=1,
    BLOCKED=2,
    FINISHED=3
}state;

typedef struct tcb{
    int id;
    state curr;
    ucontext_t* context;
    bool mutexed;
    int waiting_thread;
    int waiting_for;
    void* size_of_stack;
    void* stack;
    long time_taken;
    umutex* blocked_from_mutex;
    struct timeval start_time;
    struct timeval finish_time;
    struct timeval created_time;
    struct tcb* next_node;
}tcb;

typedef struct queue{
    tcb* head;
    tcb* tail;
};

typedef struct umutex{
    bool locked; //check if mutex is locked or not
    int id; //id of the mutex
    int owner_id; // id of the thread inside critical zone
    queue* mutex_q; // waiting queue for the mutex
};

queue* queue_init();
void enqueue(queue* q, tcb* thread);
void dequeue(queue* q,tcb** pop_node);
int remove_from_queue(queue* q,int tid,tcb** removed);
tcb* front(queue* q);
void freeQueue(queue* q);
void empty_queue(queue* q);

#endif
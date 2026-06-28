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
    int id; //thread id
    state curr; //status of the thread
    ucontext_t* context; //context/place of the thread at a time
    bool mutexed; //whether it is locked or not
    int waiting_thread; //thread which is waiting
    int waiting_for; //thread it is waiting for
    void* size_of_stack;
    void* stack;
    long time_taken; //time taken for the thread to complete the process
    umutex* blocked_from_mutex; //whether mutex has put them in blocked queue
    struct timeval start_time; 
    struct timeval finish_time;
    struct timeval created_time;
    struct tcb* next_node; //link to the next thread
}tcb;

struct queue{
    tcb* head;
    tcb* tail;
};

struct umutex{
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
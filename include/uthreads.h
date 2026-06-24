#ifndef UTHREADS_h
#define UTHREADS_H

#include<stdio.h>
#include<ucontext.h>
#include<stdlib.h>
#include <signal.h>
#include<unistd.h>
#include<time.h>
// #include<pthread.h> will include it for testing against user-threads
#include<time.h>
#include<stdbool.h>
#include<sys/time.h>
#include"list.h"
#include"queue.h"
#include"stack.h"
#include<sys/types.h>
#define TIME_SLICE 20
#define MAX 256

typedef long long ll;
typedef unsigned int uint;

// STATES
typedef enum{
    READY=0,
    RUNNING=1,
    BLOCKED=2,
    FINISHED=3
}state;

// MUTEX DECLARATION
typedef struct umutex{
    bool locked; //check if mutex is locked or not
    int id; //id of the mutex
    int owner_id; // id of the thread inside critical zone
    queue* mutex_q; // waiting queue for the mutex
}umutex;

// THREAD CONTROL BLOCK
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
    struct timeval start_time;
    struct timeval finish_time;
    struct timeval created_time;
    struct tcb* next_node;
}tcb;


#endif
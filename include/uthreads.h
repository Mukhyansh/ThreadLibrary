#ifndef UTHREADS_h
#define UTHREADS_H

#include<stdio.h>
#include<ucontext.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
// #include<pthread.h> will include it for testing against user-threads
#include<time.h>
#include<stdbool.h>
#include<sys/time.h>
#include<sys/types.h>
#define TIME_SLICE 20

typedef long long ll;
typedef unsigned int uint;

typedef enum{
    READY=0,
    RUNNING=1,
    BLOCKED=2,
    FINISHED=3
}state;

typedef struct{
    
}umutex;

typedef struct{
    int id;
    state curr;
    ucontext_t* context;
    bool mutexed;
    int waiting_thread;
    int waiting_for;
    void* size_of_stack;
    void* stack;
    long time_taken;
    long start_time;
    long finish_time;
    long created_time;
    tcb* next;
}tcb;


#endif
#ifndef UTHREADS_H
#define UTHREADS_H

#ifndef _POSIX_C_SOURCE 
#define _POSIX_C_SOURCE 200809L
#endif

#include<stdio.h>
#include<ucontext.h>
#include<stdlib.h>
#include <signal.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h> //will include it for testing against user-threads
#include<time.h>
#include<stdbool.h>
#include<sys/time.h>
#include"queue.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/signal.h>

#define USE_THIS_LIBRARY 1 //Comment this out if you want to use the pthread library for testing on the benchmarks!
#define TIME_SLICE 20
#define MAX 256

typedef long long ll;
typedef unsigned int uint;


extern tcb* all_threads[MAX]; //Useful for the join function , explained below
extern int completed[MAX]; //Useful for the join function, explained below
extern struct timeval schedule_timestamp;
extern tcb* main_thread;
extern struct itimerval timer_val; // stores the timestamps for creation,first_run and completion.
extern queue* ready_q; //queue for round robin (running)
extern queue* waiting_q; //queue for round robin (ready)
extern ucontext_t* scheduler_context; //context for the scheduler
extern ucontext_t* finished_context; //context for something after a thread has finished executing
extern tcb* running_thread;

void init();
void thread_wrapper(void*(*start)(void*),void* arg);
int thread_create(int tid,void*(*function)(void*),void*(arg));
int thread_join(int tid,void** value_pointer,queue* ready_q);
void thread_yield();
void thread_exit(void* value);
void thread_finished();
int fire_timer(int time_slice);
void schedule(); 
void scheduler_roundrobin();
int setup_handler();
void find_ready_thread(int thid,tcb** return_value,queue* ready_q);
int mutex_init(umutex* mtx);
int mutex_unlock(umutex* mtx);
int mutex_lock(umutex* mtx);
int mutex_destroy(umutex* mtx);
void unblock_threads_from_queue(umutex* mtx);
void print_stats(queue* q);
int uthread_create(int* threadid,void* attr_p,void *(*start_routine)(void *),void* arg);
int uthread_join(int tid,void** attr_p);
int umutex_init(umutex* mtx,void* attr);

//Kind off like an alias/boilerplate for my thread libary functions to act like pthread library functions
//Can be changed when the macro is commented out!


/*
Still figuring this out!
*/

// #ifdef USE_THIS_LIBRARY 

// typedef int uthread_t;
// #define pthread_t uthread_t
// #define pthread_mutex_t umutex
// #define pthread_create thread_create
// #define pthread_join uthread_join
// #define pthread_yield thread_yield
// #define pthread_exit thread_exit
// #define pthread_mutex_init umutex_init
// #define pthread_mutex_lock mutex_lock
// #define pthread_mutex_unlock mutex_unlock
// #define pthread_mutex_destroy mutex_destroy

// #endif


/*
Transferred this whole block to queue.h because of some compilation complications!


MUTEX DECLARATION
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
    
STATES
typedef enum{
    READY=0,
    RUNNING=1,
    BLOCKED=2,
    FINISHED=3
}state;
    */
    #endif
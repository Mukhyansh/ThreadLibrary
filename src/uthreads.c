#include"uthreads.h"

void init();
void thread_create(int tid,tcb* th);
void thread_join(int tid,tcb* th);
void thread_yield(int tid,tcb* th);
void thread_terminate(int tid,tcb* th);
void interrupt_handler();


tcb* main_thread;
ucontext_t* scheduler_context;
ucontext_t* finished_context;
tcb* running_thread;

int mutex_count=0;
int thread_count;
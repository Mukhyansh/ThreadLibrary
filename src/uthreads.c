#include"include/uthreads.h"
#include"include/list.h"
#include"include/queue.h"
#include"include/stack.h"

void init();
int thread_create(int tid);
void thread_join(int tid,tcb* th);
void thread_yield(int tid,tcb* th);
void thread_terminate(int tid,tcb* th);
void interrupt_handler();
void fire_timer(int time_slice);

tcb* main_thread;
ucontext_t* scheduler_context;
ucontext_t* finished_context;
tcb* running_thread;

int mutex_count=0;
int tid=1;
int thread_count=0;
int active_threads=0;

int thread_create(int tid){
    if(thread_count==0){
        init();
        puts("Round robin scheduler is running!");

        main_thread=(tcb*)malloc(sizeof(tcb));
        if(main_thread==NULL){
            printf("Error creating a thread!\n");
            return -1;
        }

        main_thread->id=tid++;
        main_thread->context=(ucontext_t*)malloc(sizeof(ucontext_t));
        if(main_thread->context==NULL){
            puts("Failed to load context from memory!");
            return -1;
        }
        if(getcontext(main_thread)==-1){
            puts("Failed to get context for the main thread!");
            return -1;
        }

        main_thread->stack=main_thread->context->uc_stack.ss_sp;
        main_thread->context->uc_link=finished_context;
        main_thread->curr=RUNNING;
        
        // active_threads++;

    }
    else{
        tcb* new_thread=(tcb*)malloc(sizeof(tcb));
    }
}
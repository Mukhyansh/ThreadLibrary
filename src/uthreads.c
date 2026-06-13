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
int fire_timer(int time_slice);

tcb* main_thread;
struct itimerval timer_val;
queue* ready_q; //queue for round robin
ucontext_t* scheduler_context;
ucontext_t* finished_context;
tcb* running_thread;

static double turnaround_avg;
static double response_avg;
int mutex_count=0;
int tid=1;
int thread_count=0;
int active_threads=0;

int fire_timer(int time_slice){
    timer_val.it_value.tv_sec=0;
    timer_val.it_value.tv_usec=(time_slice*1000) % 100000;
    //Convert to microseconds as itimer accepts microseconds not milli
    timer_val.it_interval.tv_sec=0;
    timer_val.it_interval.tv_usec=(time_slice*1000) % 100000;

    if(setitimer(ITIMER_PROF, &timer_val,NULL)==-1){
        puts("Setting up the timer failed!");
        return -1;
    }
    return 0;
}

int thread_create(int tid){
    if(thread_count==0){
        init(); //start everything
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
        //We are using the program's default'ly (invented word)
        //allocated stack for the first initial thread for better 
        //understanding. 
        //We can create stack for the initial thread as well using malloc
        //and it wouldnt make a difference!
        main_thread->context->uc_link=finished_context;
        main_thread->curr=RUNNING;
        main_thread->mutexed=false;
        main_thread->waiting_thread=-1;
        main_thread->waiting_for=-1;
        main_thread->time_taken=0;

        gettimeoftheday(&main_thread->created_time,NULL); 
        gettimeoftheday(&main_thread->start_time,NULL);

        turnaround_avg=0; // completion time - start time
        response_avg=0; // first run time - created time
        active_threads++;
        fire_timer(TIME_SLICE);
    }
    //When more than 1 thread!
    tcb *thread = (tcb*)malloc(sizeof(tcb));
	if(thread == NULL){
		printf("Failed to allocate memory for tcb of thread\n");
		return -1;
	}
    thread->context=(ucontext_t*)malloc(sizeof(ucontext_t));
	if(thread->context==NULL){
		printf("Failure to allocate memory for context\n");
		return -1;
	}
	thread->curr=READY;
	thread->id=tid++;

    thread->stack=malloc(8096);
	if(thread->stack==NULL){
		printf("Failed to allocate memory for stack\n");
		return -1;
	}

    if(getcontext(thread->context)==-1){
		printf("Failed to get the context\n");
		return -1;
	}

    thread->context->uc_stack.ss_sp=thread->stack;
	thread->context->uc_stack.ss_size=8096;
    thread->context->uc_stack.ss_flags=0;
	thread->waiting_thread=-1;
	thread->waiting_for=-1;  
    gettimeofday(&thread->start_time, NULL);  
}
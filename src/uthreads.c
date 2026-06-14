#include"include/uthreads.h"
#include"include/list.h"
#include"include/queue.h"
#include"include/stack.h"

void init(); //starts everything
void thread_wrapper(void*(*start)(void*),void* arg); //kinda like a boilerplate for all the benchmarks
int thread_create(int tid,void*(*function)(void*),void*(arg));
void thread_join(int tid,tcb* th);
void thread_yield(int tid,tcb* th);
void thread_terminate(int tid);
void interrupt_handler();
int fire_timer(int time_slice);

tcb* main_thread;
struct itimerval timer_val; // stores the timestamps for creation,first_run and completion.
queue* ready_q; //queue for round robin
queue* waiting_q;
ucontext_t* scheduler_context;
ucontext_t* finished_context;
tcb* running_thread;

static double turnaround_avg; // completion time - start time
static double response_avg; // first run time - created time
int mutex_count=0;
int tid=1; // thread id
int thread_count=0;
int active_threads=0;

void thread_wrapper(void*(*start)(void*),void* arg){
    start((void*)arg);
    printf("Thread with the ID %d exiting!\n",running_thread->id);
    thread_terminate(running_thread->id);
}

int fire_timer(int time_slice){
    timer_val.it_value.tv_sec=0;
    timer_val.it_value.tv_usec=(time_slice*1000) % 100000;
    //  Convert to microseconds as itimer accepts microseconds not milli
    timer_val.it_interval.tv_sec=0;
    timer_val.it_interval.tv_usec=(time_slice*1000) % 100000;

    if(setitimer(ITIMER_PROF, &timer_val,NULL)==-1){
        puts("Setting up the timer failed!");
        return -1;
    }
    return 0;
}

int thread_create(int thid,void*(*function)(void*),void*(arg)){
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
        if(getcontext(main_thread->context)==-1){
            puts("Failed to get context for the main thread!");
            return -1;
        }

        main_thread->stack=main_thread->context->uc_stack.ss_sp;
        //We are using the program's default'ly (i invented this word)
        //allocated stack for the first initial thread for better 
        //understanding. 
        //We can create stack for the initial thread as well using malloc
        //and it would'nt make a difference!
        main_thread->context->uc_link=finished_context;
        main_thread->curr=RUNNING;
        main_thread->mutexed=false;
        main_thread->waiting_thread=-1;
        main_thread->waiting_for=-1;
        main_thread->time_taken=0;

        gettimeofday(&main_thread->created_time,NULL); 
        gettimeofday(&main_thread->start_time,NULL);

        turnaround_avg=0; 
        response_avg=0; 
        active_threads++;
        // fire_timer(TIME_SLICE); //Will call this during init as this is causing the threads to loop back around infinitely
    }
    //  **When more than 1 thread!**
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

    thread->stack=malloc(8096); //We allocated seperate stack for the rest of the threads!
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
    thread->time_taken=0;
    thread->mutexed=false;
    gettimeofday(&thread->created_time,NULL);
    gettimeofday(&thread->start_time, NULL);  

    turnaround_avg=0; 
    response_avg=0; 
    active_threads++;
    
    enqueue(ready_q,thread);

    makecontext(&thread->context,(void*)thread_wrapper,2,function,arg);
    return thread->id;
}
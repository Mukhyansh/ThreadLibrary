#include"include/uthreads.h"

void init(); //starts everything
void thread_wrapper(void*(*start)(void*),void* arg); //kinda like a boilerplate for all the Metrics
int thread_create(int tid,void*(*function)(void*),void*(arg));
void thread_join(int tid,tcb* th);
void thread_yield(int tid);
void thread_exit(void* value);
void thread_terminate();
void interrupt_handler();
int fire_timer(int time_slice);
void schedule();
struct timeval schedule_timestamp;
void scheduler_roundrobin();
void find_ready_threads(int thid,tcb** return_value,queue* ready_q);

tcb* main_thread;
struct itimerval timer_val; // stores the timestamps for creation,first_run and completion.
queue* ready_q; //queue for round robin (running)
queue* waiting_q; //queue for round robin (ready)
ucontext_t* scheduler_context; //context for the scheduler
ucontext_t* finished_context; //context for something after a thread has finished executing
tcb* running_thread;

static double turnaround_avg; // completion time - start time
static double response_avg; // first run time - created time
int mutex_count=0;
int tid=1; // thread id
int thread_count=0;
int active_threads=0;
void* returnables[MAX];

void thread_wrapper(void*(*start)(void*),void* arg){
    start((void*)arg);
    printf("Thread with the ID %d exiting!\n",running_thread->id);
    thread_terminate();
}

int fire_timer(int time_slice){
    timer_val.it_value.tv_sec = time_slice / 1000;
    timer_val.it_value.tv_usec = (time_slice % 1000) * 1000;
    //  Convert to microseconds as itimer accepts microseconds not milli
    timer_val.it_interval.tv_sec = time_slice / 1000;
    timer_val.it_interval.tv_usec = (time_slice % 1000) * 1000;

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
        /*
        We are using the program's default'ly (i invented this word)
        allocated stack for the first initial thread for better 
        understanding. 
        We can create stack for the initial thread as well using malloc
        and it wouldn't make a difference!
        */
        main_thread->context->uc_link=finished_context;
        main_thread->curr=RUNNING;
        main_thread->mutexed=false;
        main_thread->waiting_thread=-1;
        main_thread->waiting_for=-1;
        main_thread->time_taken=0;

        gettimeofday(&main_thread->created_time,NULL); 
        gettimeofday(&main_thread->start_time,NULL);

        main_thread->curr=RUNNING;
        running_thread=main_thread;
        gettimeofday(&schedule_timestamp, NULL);

        turnaround_avg=0; 
        response_avg=0; 
        active_threads++;
        thread_count++;
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
    thread->context->uc_link = finished_context;
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
    thread_count++;
    
    enqueue(ready_q,thread);

    makecontext(thread->context,(void*)thread_wrapper,2,function,arg);
    return thread->id;
}

void init(){
    // Initialize the ready and waiting queue for the round robin scheduler!
    ready_q=queue_init();
    waiting_q=queue_init();

    // Initialize the context for scheduler.
    scheduler_context=(ucontext_t*)malloc(sizeof(ucontext_t));
    scheduler_context->uc_stack.ss_sp=(void*)malloc(8096);
    if(getcontext(scheduler_context)==-1){
        puts("Failed the initialization for scheduler's context!");
        return;
    }
    scheduler_context->uc_link=0;
    scheduler_context->uc_stack.ss_size=8096;
    scheduler_context->uc_stack.ss_flags=0;

    makecontext(scheduler_context,(void*)&schedule,0);

    //initialize the finished context
    finished_context=(ucontext_t*)malloc(sizeof(ucontext_t));
    finished_context->uc_stack.ss_sp=(void*)malloc(8096);
    if(getcontext(finished_context)==-1){
        puts("Failed the initialization of the finished context!");
        return;
    }
    finished_context->uc_link=0;
    finished_context->uc_stack.ss_size=8096;
    finished_context->uc_stack.ss_flags=0;

    makecontext(finished_context,(void*)&thread_terminate,0);
}

void thread_terminate(){
    printf("Exiting with last thread id %d.\n",running_thread->id);
    running_thread->curr=FINISHED;
    active_threads--;
    schedule();
}

/* 
This will be called by the time handler/timer (idk)
and will work as an automatic yield function
*/ 

// Preemptive
void interrupt_handler(int signum){
    if(running_thread==NULL){
        puts("Running thread is not initialized yet!");
        return;
    }
    if(swapcontext(running_thread->context,scheduler_context)==-1){
        puts("Error switching context from thread to scheduler!");
        exit(-1);
    }
}
 
/* 
The Yield function is for manual yielding but we might not need it 
as we have built a timer which will fire every 20 ms and call interrupt handler. 
*/

// Cooperative
void thread_yield(int tid){ 
    swapcontext(running_thread->context,scheduler_context);
    return;
}

// Manually terminate a thread
void thread_exit(void* value){
    if(running_thread->curr!=RUNNING){
        puts("Something is wrong in the supposedly running thread!");
        exit(-1);
    }
    running_thread->curr=FINISHED;
    if(value!=NULL){
        returnables[running_thread->id]=value;
    }
    printf("Exiting thread with the id %d.\n",running_thread->id);
    worker_yield();
}

// Gets assigned the scheduled context and calls the required scheduler for the rescheduling of the threads!
void schedule(){
    gettimeofday(&running_thread->finish_time,NULL);

    double time_ms;
	time_ms=(double)(running_thread->finish_time.tv_sec-schedule_timestamp.tv_sec)*1000;

	if(time_ms == 0){
		time_ms+=(double)(running_thread->finish_time.tv_usec-schedule_timestamp.tv_usec) / 1000;
	}
    else{
		time_ms-=1000;
		time_ms+=(double)((1000000-schedule_timestamp.tv_usec)+running_thread->finish_time.tv_usec) / 1000;
	}
    running_thread->time_taken+=time_ms;
    scheduler_roundrobin(running_thread->context);
}

//Stores the same id threads in return_value.
void find_ready_thread(int thid,tcb** return_value,queue* ready_queue){
    tcb* temp=ready_queue->head;
    while(temp){
        if(temp->id==thid){
            *return_value=temp;
            break;
        }
        temp=temp->next_node;
    }
}

void schedule(){
    tcb* next_thread=NULL;
    tcb* prev_thread=running_thread;

    if(prev_thread->curr==FINISHED){
        free_threads(prev_thread);
        free(prev_thread);
    }
    else{
        prev_thread->curr=READY;
        enqueue(ready_q,prev_thread);
    }
    dequeue(ready_q,&next_thread);
    if(next_thread==NULL){
        puts("There are no ready threads anymore!");
        exit(0);
    }
    next_thread->curr=RUNNING;
    running_thread=next_thread;
    init_timer(TIME_SLICE);
    setcontext(next_thread->context);
}

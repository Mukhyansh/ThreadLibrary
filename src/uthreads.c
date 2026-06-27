#include"../include/uthreads.h"

void init(); //starts everything
void thread_wrapper(void*(*start)(void*),void* arg); //kinda like a boilerplate for all the Metrics
int thread_create(int tid,void*(*function)(void*),void*(arg));
int thread_join(int tid,void** value_pointer,queue* ready_q);
void thread_yield();
void thread_exit(void* value);
void thread_finished(); //I initially named it `thread_terminate` but that was confusing as this requires the finished context.
void interrupt_handler(int signum);
int fire_timer(int time_slice); //Will fire a signal every 20ms.
void schedule(); 
void scheduler_roundrobin();
int setup_handler();
void find_ready_thread(int thid,tcb** return_value,queue* ready_q);
void mutex_init(umutex* mtx);
int mutex_unlock(umutex* mtx);
int mutex_lock(umutex* mtx);
int mutex_destroy(umutex* mtx);
void unblock_threads_from_queue(umutex* mtx);
void print_stats(queue* q); //filler function for inner insights~

tcb* all_threads[MAX]; //Useful for the join function , explained below
int completed[MAX]; //Useful for the join function, explained below
struct timeval schedule_timestamp;
tcb* main_thread;
struct itimerval timer_val; // stores the timestamps for creation,first_run and completion.
queue* ready_q; //queue for round robin (running)
queue* waiting_q; //queue for round robin (ready)
ucontext_t* scheduler_context; //context for the scheduler
ucontext_t* finished_context; //context for something after a thread has finished executing
tcb* running_thread;

static double turnaround_avg; // completion time - start time
static double response_avg; // first run time - created time
static int mutex_count=0;
static int interrupt_disabled=0; //gpt suggested this, idk what this does yet.
//the upper variable is for disable and enablig signal function, thats all I know about it.
static int tid=1; // thread id
static int thread_count=0;
static int active_threads=0;
void* returnables[MAX];

void thread_wrapper(void*(*start)(void*),void* arg){
    void* result=start((void*)arg);
    printf("Thread with the ID %d exiting!\n",running_thread->id);
    /*
    thread exit is not working as i need to pass this to a finished context, it can work in
    some cases when the user wants the thread to terminate mid process.
    */
    //thread_exit(NULL); //what better to pass than NULL
    returnables[running_thread->id]=result; //this works better as this returns the program to the finished context
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
        allocated stack for the first thread for better 
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
        all_threads[main_thread->id]=main_thread;

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
    all_threads[thread->id]=thread;
    gettimeofday(&thread->created_time,NULL);
    gettimeofday(&thread->start_time, NULL);  

    turnaround_avg=0; 
    response_avg=0; 
    active_threads++;
    thread_count++;
    
    makecontext(thread->context,(void*)thread_wrapper,2,function,arg);
    
    enqueue(ready_q,thread);
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

    makecontext(finished_context,(void*)&thread_finished,0);

    if(setup_handler()==-1){
        exit(1);
    }
}

void thread_finished(){
    printf("Exiting with last thread id %d.\n",running_thread->id);
    running_thread->curr=FINISHED;
    schedule();
}

/* 
This will be called by the time handler/timer (idk)
and will work as an automatic yield function
*/ 

// Preemptive
void interrupt_handler(int signum){
    if(interrupt_disabled){
        return;
    }
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
void thread_yield(){ 
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
    thread_yield();
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
    scheduler_roundrobin();
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

void scheduler_roundrobin(){
    tcb* next_thread=NULL;
    tcb* prev_thread=running_thread;

    if(prev_thread->curr==FINISHED){
        int finished_id=prev_thread->id;

        if(prev_thread->waiting_thread!=-1){
            tcb* joiner=NULL;
            remove_from_queue(waiting_q,prev_thread->waiting_thread,&joiner);

            if(joiner!=NULL){
                joiner->curr=READY;
                joiner->waiting_for=-1;
                enqueue(ready_q,joiner);
            }
        }
        completed[finished_id] = 1;
        all_threads[finished_id] = NULL;
        active_threads--;

        if (prev_thread != main_thread) {
            free(prev_thread->stack);
            free(prev_thread->context);
            free(prev_thread);
        }
    }
    else if(prev_thread->curr==BLOCKED){
        //Likely to do nothing here
        //I name this area of the code "The Thrib("thread crib)!
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
    gettimeofday(&schedule_timestamp,NULL);
    fire_timer(TIME_SLICE);
    setcontext(next_thread->context);
} 

// Joins another thread by pausing one and leetting the second continue till the end.
int thread_join(int tid,void** value_pointer,queue* ready_q){
    tcb* waiting_t=NULL;
    /*
    the find ready thread function was only looking the status(curr) of the threads inside the ready queue
    but the threads we are looking for could very well be inside the waiting queue.
    I mean, it still makes sense to implement a basic linear search for the waiting queue but I
    think what I am doing will save a tad more time internally(idk).
    
    The `all_threads` memory block wil hold alll the threads therefore making it easy for me to lookup in O(1) time.

    */
    //find_ready_thread(tid,&waiting_t,ready_q);

    waiting_t = all_threads[tid];
    if(waiting_t==NULL){
        if(tid<=thread_count){
            if(value_pointer!=NULL){
                *value_pointer=returnables[tid];
            }
            return 0;
        }
        return -1;
    }
    if(waiting_t->waiting_thread>0){
        printf("The thread %d has already been joined by another thread!",tid);
        return -1;
    }
    waiting_t->waiting_thread=running_thread->id;
    running_thread->curr=BLOCKED;
    running_thread->waiting_for=waiting_t->id;
    enqueue(waiting_q,running_thread);
    thread_yield();

    if(*value_pointer!=NULL){
        *value_pointer=returnables[tid];
    }

    return 0;
}

int setup_handler(){
    struct sigaction ac;

    ac.sa_flags=0;
    ac.sa_handler=interrupt_handler;
    if(sigaction(SIGPROF,&ac,NULL)==-1){
        puts("Error at sigaction in sigaction!");
        return -1;
    }
    return 1;
}

//Self explanatory :/

void disable_signal(){
    interrupt_disabled=1;
}

void enable_signal(){
    interrupt_disabled=0;
}

//Initializes the mutex!
void mutex_init(umutex* mtx){
    if(!mtx){
        printf("Error creating mutex!\n");
        exit(EXIT_FAILURE);
    }   
    mtx->id=mutex_count;
    mtx->locked=0;
    mtx->owner_id=-1;
    mtx->mutex_q=queue_init();
    mutex_count++;
}

//Locks the mutex so that only one thread can access it at a time.
int mutex_lock(umutex *mtx){
    if(mtx==NULL || mtx->mutex_q==NULL){
        return -1;
    }

    while(1){
        disable_signal();
        if(__sync_lock_test_and_set(&mtx->locked, 1)==0){
            mtx->owner_id=running_thread->id;
            running_thread->mutexed=false;
            running_thread->blocked_from_mutex=NULL;
            enable_signal();
            return 0;
        }

        if(mtx->owner_id==running_thread->id){
            enable_signal();
            return -1;  // non-recursive mutex
        }

        running_thread->curr=BLOCKED;
        running_thread->waiting_for=mtx->owner_id;
        running_thread->mutexed=true;
        running_thread->blocked_from_mutex=mtx;

        enqueue(mtx->mutex_q, running_thread);// only this queue
        enable_signal();
        thread_yield();
    }
}

//Unlocks the mutex so that other threads can access it.
int mutex_unlock(umutex* mtx){
    if(!mtx){
        printf("Mutex has not been initialized!\n");
        return -1;
    }
    if(mtx->locked){
        if(running_thread->id != mtx->owner_id){
            puts("Mutex is locked by another thread!");
            return -1;
        }
        mtx->owner_id=-1;
        __sync_lock_release(&mtx->locked);
        if(mtx->mutex_q->head!=NULL){
            unblock_threads_from_queue(mtx);
        }
    }
    else{
        puts("Mutex is not even locked!");
        return -1;
    }   
    return 0;
}

/*
 This function picks the blocked thread from the waiting queue and sets them to ready and puts them in the ready queue.(mutex)
 This function is designed to pick up only one thread at a time and push them into the ready queue!
*/
void unblock_threads_from_queue(umutex* mtx){
    if(!mtx || !mtx->mutex_q){
        puts("error in unblock threads function!");
        return;
    }
    tcb* v_hold=NULL; //to hold the dequeue value
    dequeue(mtx->mutex_q,&v_hold);

    if(v_hold==NULL){
        puts("Initialization for the value_holder failed!");
        return;
    }

    v_hold->curr=READY;
    v_hold->mutexed=false;
    v_hold->blocked_from_mutex=NULL;
    v_hold->waiting_for=-1;

    enqueue(ready_q,v_hold);
}

// Self explanatory :/
int mutex_destroy(umutex* mtx){
    if(mtx==NULL){
        puts("It is already NULL, what do you need me for?");
        return -1;
    }
    if(mtx->mutex_q==NULL){
        puts("Queue is NULL!");
        return -1;
    }
    if(mtx->locked){
        puts("Locked by another thread, can't be destroyed!");
        return -1;
    }

    free(mtx->mutex_q);
    mtx=NULL;
    return 0;
}

//prints the stats of the thread in any queue which has been passed as an argument!
void print_stats(queue* q){
    tcb* temp=q->head;
    int i=0;
    while(temp!=NULL){
        printf("Thread %d is at the position: %d and is waiting for: %d.\n",temp->id,i,temp->waiting_for);
        temp=temp->next_node;
        i++;
    }
}


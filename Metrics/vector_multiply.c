#include"../include/uthreads.h"

//Before going any further, please checkout the README files!

#define VECTOR_SIZE 100000
#define THREAD_NUM 3

static long long right[VECTOR_SIZE];
static long long left[VECTOR_SIZE];
static long long final_sum=0;

static umutex result_mutex;

static int thread_ids[THREAD_NUM];
static int thread_args[THREAD_NUM];

//Main logic for calculating the result using threads
void* calculate(void* arg){
    int start=*(int*)arg;
    long long part_sum=0;
    for(int i=start;i<VECTOR_SIZE;i++){
        part_sum+=right[i]*left[i];
    }
    pthread_mutex_lock(&result_mutex);
    final_sum++;
    pthread_mutex_unlock(&result_mutex);
    
    return NULL;
}

//Verify the actual time by manually calculating without threads!
long long verify(){
    long long sum=0;
    for(int i=0;i<VECTOR_SIZE;i++){
        sum+=right[i]*left[i];
    }
    return sum;
}

//calculates the time difference between start and end in microseconds
long long elapsed_time(struct timespec start,struct timespec end){
    return (end.tv_sec-start.tv_sec)*1000000LL + (end.tv_nsec-start.tv_nsec)/1000;
}

int main(int argc,char* argv[]){
    struct timespec start,end;

    /*
    We are gonna get some return value from the calculation.
    We donot actually need it so we are  putting it inside this and ignoring it(like u) lol.
    */
   void* ignored_return_value=NULL; 
   
    for(int i=0;i<VECTOR_SIZE;i++){
        //filling up the vector with default random values
        //you can manually add the values for 1 MILLION places OFCOURSE(psycho).
        right[i]=i;
        left[i]=i;
    }

    pthread_mutex_init(&result_mutex,NULL);

    clock_gettime(CLOCK_MONOTONIC,&start);

    for(int i=0;i<THREAD_NUM;i++){
        thread_args[i]=i;
        //lol my current thread_create function generate its own id so we donot need to worry about the first argument!
        thread_ids[i]=pthread_create(0,NULL,calculate,&thread_args);
        if(thread_ids[i]==-1){
            puts("Thread creation failed!");
            return -1;
        }
    }
    /*
    The first join blocks main and yields to the scheduler, 
    allowing the worker threads to start running.
    */
    for(int i=0;i<THREAD_NUM;i++){
        if(pthread_join(thread_args[i],&ignored_return_value)==-1){
            puts("Thread join failed!");
            return -1;
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC,&end);
    long long actual=verify();

    printf("Parallel Result using Threads: %lld.\n",final_sum);
    printf("Verified sum by manual calculation is: %lld.\n",actual);
    printf("Running Time is: %lld microseconds.\n",elapsed_time);
    if(final_sum==actual){
        printf("Mutex and the thread library worked correctly!\n");
    }
    else{
        puts("They failed!");
    }
    pthread_mutex_destroy(&result_mutex);
    return 0;
    
}
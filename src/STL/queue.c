#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/uthreads.h"
#include "../include/queue.h"


queue* queue_init(){
    queue* q=(queue*)malloc(sizeof(queue));
    q->head=NULL;
    q->tail=NULL;
    return q;
}

void empty_queue(queue* q){
	q->head=NULL;
	q->tail=NULL;
}

void enqueue(queue* q, tcb* thread){
    thread->next_node = NULL;
    if (q->head==NULL) {
        q->head=thread;
        q->tail=thread;
    } else {
        q->tail->next_node=thread;
        q->tail=thread;
    }
}

tcb* dequeue(queue* q){
    if(q->head==NULL){
        return NULL;
    }
    tcb* thread=q->head;
    q->head=q->head->next_node;
    if(q->head==NULL){
        q->tail=NULL;
    }
    return thread;
}

tcb* front(queue* q){
    if(q->head==NULL){
        puts("Queue empty!");
        return NULL;
    }
    return q->head;
}

void freeQueue(queue* q){
    while(q->head!=NULL){
        tcb* temp=q->head;
        q->head=q->head->next_node;
    }
    free(q);
}


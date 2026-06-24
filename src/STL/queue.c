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

void dequeue(queue* q,tcb** pop_node){
    if(q->head==NULL){
        return;
    }
    *pop_node=q->head;
    tcb* thread=q->head->next_node;
    q->head->next_node=NULL;
    if(q->head==q->tail){
        q->tail=NULL;
        q->head=NULL;
    }
    else{
        q->head=thread;
    }
}

int remove(queue* q,int tid,tcb** pop_node){
    if(q->head==NULL){
        return NULL;
    }
    *pop_node=NULL;
    tcb* current=q->head;
    tcb* previous=NULL;
    
    while(current!=NULL){
        if(current->id==tid){
            break;
        }
        previous=current;
        current=current->next_node;
    }

    if(current==NULL){
        return -1;
    }

    if(previous==NULL){
        q->head=current->next_node;
    }
    else{
        previous->next_node=current->next_node;
    }

    if(q->tail==current){
        q->tail=previous;
    }
    if(q->head==NULL){
        q->tail=NULL;
    }

    current->next_node=NULL;
    *pop_node=current;
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


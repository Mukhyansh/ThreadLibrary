#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct{
    void* arr[8096];
    int front;
    int end;
}queue;

queue* init(){
    queue* q=(queue*)malloc(sizeof(queue));
    q->front=0;
    q->end=0;
    return q;
}

void push(queue* q,void* ch){
    if((q->end+1)%8096==q->front){
        puts("Queue full!");
        return;
    }
    q->arr[q->end]=ch;
    q->end=(q->end+1)%8096;
}

void* pop(queue* q){
    if(q->front==q->end){
        puts("Queue empty!");
        return NULL;
    }
    void* data=q->arr[q->front];
    q->front=(q->front+1)%8096;
    return data;
}

void* front(queue* q){
    if(q->front==q->end){
        puts("Queue empty!");
        return NULL;
    }
    return q->arr[q->front];
}

void freeQueue(queue* q){
    free(q);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node{
    void* data;
    struct node* next;
}node;

typedef struct{
    node* head;
    node* tail;
}queue;

queue* init(){
    queue* q=(queue*)malloc(sizeof(queue));
    q->head=NULL;
    q->tail=NULL;
    return q;
}

void push(queue* q,void* ch){
    node* newNode=(node*)malloc(sizeof(node));
    newNode->data=ch;
    newNode->next=NULL;
    
    if(q->tail==NULL){
        q->head=newNode;
        q->tail=newNode;
    }else{
        q->tail->next=newNode;
        q->tail=newNode;
    }
}

void* pop(queue* q){
    if(q->head==NULL){
        puts("Queue empty!");
        return NULL;
    }
    void* data=q->head->data;
    node* temp=q->head;
    q->head=q->head->next;
    
    if(q->head==NULL){
        q->tail=NULL;
    }
    
    free(temp);
    return data;
}

void* front(queue* q){
    if(q->head==NULL){
        puts("Queue empty!");
        return NULL;
    }
    return q->head->data;
}

void freeQueue(queue* q){
    while(q->head!=NULL){
        node* temp=q->head;
        q->head=q->head->next;
        free(temp);
    }
    free(q);
}


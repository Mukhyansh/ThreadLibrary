//This was maybe used idk i forgot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct{
    void* arr[8096];
    int top;
}stack;

stack* init(){
    stack* st=(stack*)malloc(sizeof(stack));
    st->top=-1;
    return st;
}

void push(stack* st,void* ch){
    if(st->top==8095){
        puts("Stack full!");
        return;
    }
    st->arr[++(st->top)]=ch;
}

void* pop(stack* st){
    if(st->top<0){
        puts("Nothing to pop!");
        return;
    }
    return st->arr[st->top--];
}

void* top(stack* st){
    if(st->top<0){
        puts("Nothing to show!");
        return;
    }
    return st->arr[st->top];
}

void freeStack(stack* st){
    free(st);
}


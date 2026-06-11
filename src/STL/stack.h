#ifndef STACK_H
#define STACK_H

typedef struct{
    void* arr[8096];
    int top;
}stack;

stack* init();
void push(stack* st,void* ch);
void* pop(stack* st);
void* top(stack* st);
void freeStack(stack* st);

#endif
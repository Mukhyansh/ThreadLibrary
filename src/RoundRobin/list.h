#ifndef LIST_H
#define LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>
#include<unistd.h>

typedef struct node{
    int pid;
    int burst_time;
    int waiting_time;
    int remaining_time;
    int completion_time;
    struct node* next;
}node;

// Function declarations
node* insertatbegin(node* head,int val);
node* insertatend(node* head,int val);
node* insertatpos(node* head,int val,int x);
node* deletefrombegin(node* head);
node* deletefromend(node* head);
node* deletefrompos(node* head,int x);
node* turntocircular(node* head);
void display(node* head);
int leng(node* head);

#endif

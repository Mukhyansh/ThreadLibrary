//this was not used at all

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>
#include<unistd.h>

typedef struct node{
    int data;
    struct node* next;
}node;

node* insertatbegin(node* head,int val){
    node* temp=(node*)malloc(sizeof(node));
    if(!temp){
        printf("Failed!\n");
        exit(0);
    }
    else{
        temp->data=val;
        temp->next=head;
        head=temp;
    }
    return head;
}

node* insertatend(node* head,int val){
    node* temp=(node*)malloc(sizeof(node));
    if(!temp){
        printf("Failed!\n");
        exit(0);
    }
    else{
        if(head==NULL){
            temp->data=val;
        temp->next=head;
        head=temp;
        }
        else{
            node* p1=head;
            while(p1->next){
                p1=p1->next;
            }
            temp->data=val;
            p1->next=temp;
            temp->next=NULL;
        }
    }
    return head;
}

node* insertatpos(node* head,int val,int x){
    node* temp=(node*)malloc(sizeof(node));
    if(!temp){
        printf("Failed!\n");
        exit(0);
    }
    else{
        if(x==0){
            temp->data=val;
            temp->next=head;
            return temp;
        }
        if(x>leng(head)){
            printf("Wring position entered!");
            exit(0);
        }
        else{
            node* t=head;
            int i=0;
            while(i<x-1){
                t=t->next;
                i++;
            }
            temp->data=val;
            temp->next=t->next;
            t->next=temp;
        }
    }
    return head;
}

node* deletefrombegin(node* head){
    if(!head){
        printf("Underflow!\n");
        exit(0);
    }
    else{
        node* temp=head;
        head=head->next;
        free(temp);
    }
    return head;
}

node* deletefromend(node* head){
    if(!head){
        printf("Underflow!\n");
        exit(0);
    }
    else if(!head->next){
        free(head);
        return NULL;
    }
    else{
        node* t=head;
        while(t->next->next){
            t=t->next;
        }
        free(t->next);
        t->next=NULL;
    }
    return head;
}

node* deletefrompos(node* head,int x){
    if(!head){
        printf("Underflow!\n");
        exit(0);
    }
    else{
        if(!head){
            printf("Underflow!\n");
            exit(0);
        }
        else if(x>leng(head)){
            printf("Wrong position entered!\n");
            exit(0);
        }
        else{
            int i=0;
            node* t=head;
            while(i<x-1){
                t=t->next;
                i++;
            }
            node* o=t->next;
            t->next=t->next->next;
            free(o);
        }
    }
    return head;
}

node* turntocircular(node* head){
    node* temp=head;
    while(temp->next){
        temp=temp->next;
    }
    temp->next=head;
    return head;
}

void display(node* head){
    if(!head){
        printf("Underflow!\n");
        exit(0);
    }
    else{
        node* temp=head;
        while(temp->next!=head){
            printf("%d ",temp->data);
            temp=temp->next;
        }
        puts("");
    }
}

int leng(node* head){
    int count=0;
    node* temp=head;
    while(temp){
        count++;
        temp=temp->next;
    }
    return count;
}

int main(){
    node* head1=NULL;
    node* head2=NULL;
    return 0;
}
 

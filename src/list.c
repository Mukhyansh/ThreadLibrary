#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<stdbool.h>

typedef struct node{
    int data;
    struct node* next;
}node;

int leng(node* head){
    node* temp=head;
    int count=0;
    while(temp){
        temp=temp->next;
        count++;
    }
    return count;
}

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
    else{
        node* t=head;
        while(t->next->next){
            t=t->next;
        }
        free(t->next);
        t->next=t->next->next;
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

node* merge(node* head1,node* head2){
    node* dum=(node*)malloc(sizeof(node));
    dum->next=NULL;
    if(!head1){
        return head2;
    }
    if(!head2){
        return head1;
    }
        node* t1=head1;
        node* t2=head2;
        node* tail=dum;
        while(t1 && t2){
            if(t1->data <= t2->data){
                tail->next=t1;
                t1=t1->next;
                tail=tail->next;
            }
            else{
                tail->next=t2;
                t2=t2->next;
                tail=tail->next;
            }
        }
        if(t1) tail->next=t1;
        else if(t2) tail->next=t2;


    node* result=dum->next;
    free(dum);
    return result;
}

node* rotate(node* head,int k){
    node* newhead;
    // k=k%leng(head);
    if(k==0){
        return head;
    }
    if(!head){
        printf("Underflow!\n");
        exit(0);
    }
    else{
        int i=0;
        node* t1=head;
        while(i<k){
            t1=t1->next;
            i++;
        }
        node* t2=head;
        while(t2->next){
            t2=t2->next;
        }
        newhead=t1->next;
        t1->next=NULL;
        t2->next=head;
    }
    return newhead;
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
        while(temp){
            printf("%d ",temp->data);
            temp=temp->next;
        }
        puts("");
    }
}

node* deletefromsorted(node* head){
    node* temp=head;
    while(temp->next!=NULL){
        if(temp->data==temp->next->data){
            node* del=temp->next;
            temp->next=temp->next->next;
            free(del);
        }
        else
        temp=temp->next;
    }
    return head;
}

int main(){
    node* head1=NULL;
    node* head2=NULL;
    return 0;
}


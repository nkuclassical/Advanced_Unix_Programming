/*
  stack.c
  finalxcode

  Created by Yanqiao Zhan on 11/26/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#include "stack.h"

int count=0;
void stack_init(){
    head=NULL;
}
int isEmpty(){
    if(head==NULL)return 1;
    else return 0;
}
char* pop(){/*get the top element and remove it*/
    struct node *head1=head;
    if(head1==NULL){
        return NULL;
    }else{
        head=head->next;
        count--;
        return head1->subpath;
    }

}
char* top(){/*get the top element without remove it*/
    struct node *head1=head;
    if(head1==NULL){
        return NULL;
    }else{
        return head1->subpath;
    }
}
void push(char*path){
    if(isEmpty()){
        head=(struct node*)malloc(sizeof(struct node));
        head->next=NULL;
        head->subpath=path;
    }else{
        struct node* temp=(struct node*)malloc(sizeof(struct node));
        temp->next=head;
        temp->subpath=path;
        head=temp;
    }
    count++;
}
int size(){
    return count;
}


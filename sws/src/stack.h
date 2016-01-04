/*
  stack.h
  finalxcode

  Created by Yanqiao Zhan on 11/26/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef stack_h
#define stack_h

#include <stdio.h>
#include <stdlib.h>

struct node{
    char*subpath;
    struct node *next;
}*head;

void stack_init();
extern int isEmpty();
char* pop();/*get the top element and remove it*/
char* top();/*get the top element without remove it*/
void push(char*path);
int size();

#endif /* stack_h */

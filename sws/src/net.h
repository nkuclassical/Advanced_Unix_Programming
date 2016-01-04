/*
  net.h
  finalxcode

  Created by Yanqiao Zhan on 11/25/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef net_h
#define net_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <time.h>
#include "stack.h"
#include "process.h"
#include "define.h"

int server_listen(NetArg *);
void parent_wait(int);
int handle(char*,NetArg*,char*,int);
char* simplifyPath(char*);
char* adjunct(char *, char *);
int ipaddressvalid(char*);
#endif /* net_h */

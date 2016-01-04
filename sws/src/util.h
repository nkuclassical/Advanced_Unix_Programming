/*
  util.h
  finalxcode

  Created by Yanqiao Zhan on 11/29/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef util_h
#define util_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "define.h"


/* 
*	read the argv[0] as Status Code 
*	change argv[1] to  Reason Phrase 
*   return 0 if success, return -1 if Status Code not exists
*
*/
int getStatusDetail(int, char* );


int sendtoClient(NetRequest*,int);
int sendtoClientContent(NetRequest*,char*);
int sendtoLog(NetArg*,NetRequest*,char*);
int sendtoServer(NetArg*,NetRequest*,char*);
int sendtoClientHead(NetArg*,NetRequest*,char*);
int updateLog(NetArg*,char*);
#endif /* util_h */

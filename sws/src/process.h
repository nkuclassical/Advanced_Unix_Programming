/*
  process.h
  finalxcode

  Created by Yanqiao Zhan on 11/28/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef process_h
#define process_h

#include <stdio.h>
#include "cgi.h"
#include "normalfile.h"
#include "dir.h"
#include "define.h"

/**
    NetArg* is argument set of server
    NetRequest* is argument set from client
    int is the log file descriptor
 
    return value -1 -> fail   1 -> true  0 -> undefined
 */
/* int proc(NetArg*,NetRequest*);*/
int proc(NetArg*,NetRequest*,char*);
#endif /* process_h */

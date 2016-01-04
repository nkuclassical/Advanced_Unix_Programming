/*
  cgi.h
  finalxcode

  Created by Yanqiao Zhan on 11/25/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef cgi_h
#define cgi_h

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "define.h"
#include "util.h"
int cgiexe(NetRequest*,NetArg*,char*);
#endif /* cgi_h */
